# Ansible Example

This example demonstrates running Hestia in a multi-node environment with Ansible.

## Installing Ansible

This is based on: https://docs.ansible.com/ansible/latest/getting_started/index.html

Ansible should be installed on some 'control node' - this can be your local machine for testing.

```sh
python3 -m pip install --user ansible
```

Depending on platform you may need to add the local Python binary dir to your path, e.g. on Mac:

```sh
export PATH=$HOME/Library/Python/3.11/bin:$PATH
```

and check with:

```sh
ansible --version
```

## Setting up managed nodes

Managed nodes are some 'remote' system or host that Ansible can control. It could be a bunch of VMs but docker/podman containers work too.

If you want to set up your own VMs you can create one for each of the ips needed in [inventory/hosts.yaml](./inventory/hosts.yml) - making sure that there is a a useable Python interpeter and user added who can ssh into the machine with a keypair. 

For convenience this example includes an extra Playbook to dynamically set up some containers to test on, to use it we need `docker` or `podman` installed on the host system, as well as the `docker` python library:

```sh
python3 -m pip install --user docker
```

