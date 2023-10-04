# Ansible Example

This example demonstrates running Hestia in a multi-node environment with Ansible.

## Installing Ansible

This is based on: https://docs.ansible.com/ansible/latest/getting_started/index.html

Ansible should be installed on some 'control node' - this can be a Docker container to allow for easy networking. A local installation of Ansible is not recommended when using containers on MacOS as containers are run in an isolated Linux VM managed by Docker/Podman Desktop. 

If using a Linux system, Ansible can be installed with:

```sh
python3 -m pip install --user ansible
```

To find your container's IP addresses (to add to [inventory/hosts.yaml](./inventory/hosts.yml)):

```sh
docker network inspect hestia_network
```

## Setting up managed nodes

Managed nodes are some 'remote' system or host that Ansible can control. It could be a bunch of VMs but docker/podman containers work well. For testing purposes we have provided a Dockerfile - `containers/base/Dockerfile`.

If you want to set up your own VMs you can create one for each of the IPs needed in [inventory/hosts.yaml](./inventory/hosts.yml) - making sure that there is a a useable Python interpeter and user added who can ssh into the machine with a keypair.

The managed nodes listed in `hosts.yml` are all of type `file` and `file_hsm`, Hestia's test POSIX backend. Sample configurations are provided for the mocked versions of Motr and Phobos also, which can be installed via in the `hestia-tests` RPM (compiled by running `make package` on a hestia build with tests enabled).

For convenience this example includes a Playbook to dynamically provision your containers to test on, to use it we need `docker` or `podman` installed on the host system, as well as the `docker` python library.

```sh
python3 -m pip install --user docker
```

For MacOS run these [extra steps](#troubleshooting).

To setup, start, and provision the example nodes via Ansible, will need the Hestia source and the RPMs you intend on using. Then run:

```sh
python3 hestia_ansible.py setup
--hestia_rpm_dir=PATH_TO_HESTIA_RPMS --hestia_src_dir=PATH_TO_HESTIA_SRC
```

This will create containers in a Docker network cluster named "hestia_network" as follows:
  - `hestia_client` - A client node which is part of the network - ssh access will be exposed on your host as port 2222 - to execute commands use this endpoint. 
  - `hestia_controller_0` - Hestia controller node 
  - `hestia_worker_0` - Hestia example worker 0 (2 tiers).
  - `hestia_worker_1` - Hestia example worker 1 (3 tiers).

Auxiliary containers:
  - `hestia_ansible_controller` - Provisioning is carried out from this node.
  - `hestia_repo` - Simple http web server used as an RPM repository for provisioning.

This cluster should now be started. To test this cluster on your host machine:

```sh
# Connect to the hestia client
ssh ansible_user@localhost -p 2222
# Create an object and put some data via the cluster's Hestia controller
echo "test" > test.txt
hestia object create --host hestia_controller_0 
hestia object put_data --host hestia_controller_0 --file test.txt
```

To save/print debug logs from the Hestia containers:

```sh
python3 hestia_ansible.py logs --logfile OPTIONAL_LOGFILE
```

To stop all used containers and images:

```sh
python3 hestia_ansible.py stop
```

To start them again run:

```sh
python3 hestia_ansible.py start
```

To stop and delete created objects:

```sh
python3 hestia_ansible.py cleanup
```

### Troubleshooting

To run these on MacOS you may need to set your `DOCKER_HOST` environment variable for this library to work:

```sh
docker context list 
# NAME                TYPE                DESCRIPTION                               DOCKER ENDPOINT                                 KUBERNETES ENDPOINT   ORCHESTRATOR
# default *           moby                Current DOCKER_HOST based configuration   unix:///Users/youruser/.docker/run/docker.sock                         
# desktop-linux       moby                Docker Desktop                            unix:///Users/youruser/.docker/run/docker.sock 
# Use the output to provide the value for:
export DOCKER_HOST=unix:///Users/myuser/.docker/run/docker.sock
```

### Manual setup steps

These are provided as an alternative to the Python script, if you wish to see the docker commands run to create the test cluster. They should lead to the same result as the Python script run with default settings.
#### Setting up an RPM repository for Hestia

In order to use Hestia's Ansible setup roles, you will need to have the Hestia RPMs in a RPM repository accessible from the network of your managed nodes (VMs/Docker images which Ansible is provisioning).

To do so with Docker, place the Hestia RPMs in your working directory and run:

```sh
docker build -t hestia_repo:latest -f HESTIA_PATH/examples/sample_ansible_deploy/containers/repo/Dockerfile .
```

To start the repository server on the Docker network "hestia", and run it in the background, run:

```sh
docker network create hestia
docker run --name hestia_repo --network hestia -d hestia_repo
```

Note, to update the RPMs you will need to rebuild the image, and these repositories are unsigned.

#### Building managed nodes manually

The steps executed by the python script can be executed in the command line as:

```sh
docker build -t hestia_base:latest -f PATH_TO_HESTIA_SRC/examples/sample_ansible_deploy/containers/base/Dockerfile .
docker run --name hestia_controller_0 --network hestia -p 8080:8080 -d hestia_base # Exposes the server to the host
docker run --name hestia_worker_0 --network hestia -d hestia_base
docker run --name hestia_worker_1 --network hestia -d hestia_base
docker run --name hestia_client --network hestia -d hestia_base
```

#### Manual Provisioning with Ansible

It is recommended to run Ansible in a container, particularly if running on a MacOS host. To build the Ansible controller image:

```sh
docker build -t hestia_ansible_controller:latest -f PATH_TO_HESTIA_SRC/examples/sample_ansible_deploy/containers/ansible/Dockerfile PATH_TO_HESTIA_SRC/
```

To run the ansible provisioning automatically:

```sh
docker run --network hestia hestia_ansible_controller
```

or to manually run Ansible commands instead:

```sh
docker run --network hestia -it hestia_ansible_contoller /usr/bin/bash

# Example
pwd # /sample_ansible_deploy
ansible playbook playbook.yml -i inventory
```

The connection steps detailed for the automatic script are the same from this stage. 