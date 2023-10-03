import os
import sys
import argparse
from pathlib import Path

import docker


class AnsibleCluster():
    def __init__(self, cluster_name: str = "hestia", rpm_dir: Path = None, src_dir: Path = None, logfile: Path = None):
        print("Loading Docker environment")
        self.docker_client = docker.from_env()

        print(f"Operating on cluster with name: {cluster_name}")
        self.cluster_name = cluster_name
        self.rpm_dir = rpm_dir
        self.src_dir = src_dir
        self.logfile = logfile

        self.example_dir = src_dir / "examples/sample_ansible_deploy"
        self.docker_dir = self.example_dir / "containers"
        self.ansible_inventory = self.example_dir / "inventory"

        self.network_name = f"{cluster_name}_network"

        self.image_names = ["repo", "ansible", "base"]

        self.cluster_names = [
            f"{cluster_name}_{name}" for name in [
                "client",
                "controller_0",
                "worker_0",
                "worker_1",
            ]
        ]

        self.docker_images = {}
        self.docker_network = None
        self.docker_containers = {}

    def build_images(self):
        print("\nBuilding images:")
        for img_name in self.image_names:
            print(f"Building {img_name} image")
            self.docker_images[img_name], _ = self.docker_client.images.build(
                dockerfile=f"{self.docker_dir}/{img_name}/Dockerfile",
                path=f"{self.rpm_dir if img_name == 'repo' else self.src_dir}",
                tag=f"{self.cluster_name}_{img_name}:latest", rm=True, nocache=False
            )

    def start_cluster(self):
        print("\nStarting cluster:")
        try:
            self.docker_network = self.docker_client.networks.get(
                self.network_name)
            print(f"Found network {self.network_name}")
        except docker.errors.NotFound:
            print(f"Creating network {self.network_name}")
            self.docker_network = self.docker_client.networks.create(
                name=self.network_name)

        for name in self.cluster_names:
            try:
                self.docker_containers[name] = self.docker_client.containers.get(
                    name)
                print(f"Container {name} found, starting")
                self.docker_containers[name].start()
            except docker.errors.NotFound:
                print(f"Starting container: {name}")
                self.docker_containers[name] = self.docker_client.containers.run(
                    self.docker_images["base"].id, name=name, network=self.docker_network.id, detach=True,
                    ports={
                        '22/tcp': 2222} if name == f"{self.cluster_name}_client" else {}
                )

    def stop_containers(self):
        print("\nStopping cluster:")
        for name in self.cluster_names:
            try:
                self.docker_containers[name] = self.docker_client.containers.get(
                    name)
                print(f"Stopping the container: {name}")
                self.docker_containers[name].stop(1)
            except docker.errors.NotFound:
                pass

    def run_ansible(self):
        print("\nProvisioning nodes via Ansible")

        print(f"Starting container: {self.cluster_name}_repo")
        self.docker_client.containers.run(
            self.docker_images["repo"].id,
            name=f"{self.cluster_name}_repo",
            network=self.docker_network.id,
            detach=True
        )

        print(f"Starting container: {self.cluster_name}_ansible_controller")
        try:
            self.docker_client.containers.run(
                command="/usr/bin/ansible-playbook playbook.yml -i inventory",
                image=self.docker_images["ansible"].id,
                name=f"{self.cluster_name}_ansible_controller",
                network=self.docker_network.id
            )
        except docker.errors.ContainerError as e:
            try:
                print(
                    self.docker_client.containers.get(
                        f"{self.cluster_name}_ansible_controller").logs().decode('ascii')
                )
            except docker.errors.NotFound:
                pass
            raise e

        print("Cluster configured, connect with:\nssh ansible_user@localhost -p 2222")

    def cluster_logs(self):
        logfile = None
        
        if self.logfile is not None:
            print(f"Writing logs to {self.logfile}")
            logfile = open(self.logfile, "w")

        for name in [x for x in self.cluster_names if x != f"{self.cluster_name}_client"]:
            try:
                container = self.docker_client.containers.get(name)
                _, log = container.exec_run(
                    "/usr/bin/bash -c \"tail -v -n +1 /home/ansible_user/.cache/hestia/*.txt\"")
                log = f"Hestia Logs from {name}:\n\n" + log.decode("ascii") + "\n\n"
                if logfile is None:
                    print(log)
                else:
                    logfile.write(log)
            except docker.errors.NotFound:
                return None
        
        if logfile is not None: 
            logfile.close()

    def remove_container(self, name: str):
        try:
            container = self.docker_client.containers.get(name)
            print(f"Removing container: {name}")
            container.remove(force=True)
        except docker.errors.NotFound:
            pass

    def cleanup(self):
        print("\nDestroying containers")

        self.remove_container(f"{self.cluster_name}_ansible_controller")
        self.remove_container(f"{self.cluster_name}_repo")

        for name in self.cluster_names:
            self.remove_container(name)

        try:
            print(f"Removing network: {self.network_name}")
            self.docker_network = self.docker_client.networks.get(
                self.network_name)
            self.docker_network.remove()
        except docker.errors.NotFound:
            pass


def get_absolute_path(path: str):
    dir = Path(path)
    if not dir.is_absolute():
        dir = os.getcwd() / dir
    return dir


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument(
        'mode', choices=['setup', 'start', 'logs', 'stop', 'cleanup'])

    parser.add_argument('--hestia_rpm_dir', type=str, default="")
    parser.add_argument('--hestia_src_dir', type=str, default="")
    parser.add_argument('--cluster_name', type=str, default="hestia")
    parser.add_argument('--logfile', type=str, default=None)

    args = parser.parse_args()

    rpm_dir = get_absolute_path(args.hestia_rpm_dir)
    src_dir = get_absolute_path(args.hestia_src_dir)

    ansible_cluster = AnsibleCluster(
        args.cluster_name,
        rpm_dir,
        src_dir,
        args.logfile
    )

    if args.mode == 'setup':
        try:
            ansible_cluster.build_images()
            ansible_cluster.start_cluster()
            ansible_cluster.run_ansible()
        except Exception as error:
            print(error)
            print("Error setting up, aborting")
            prompt = input("Delete containers? (y)/n: ")
            if prompt != 'n':
                ansible_cluster.cleanup()
            sys.exit(1)

    elif args.mode == 'start':
        ansible_cluster.start_cluster()

    elif args.mode == 'logs':
        ansible_cluster.cluster_logs()

    elif args.mode == 'stop':
        ansible_cluster.stop_containers()

    elif args.mode == 'cleanup':
        ansible_cluster.cleanup()
