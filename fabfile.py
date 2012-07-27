import os
from fabric.api import *
from fabric.contrib.console import confirm
from fabric.contrib.files import exists
from fabric.context_managers import cd, settings
from fabric.api import task
from fabric.contrib.files import append
env.hosts = [
    'li493-116.members.linode.com',
    ]

arch_packages = [
        "zsh","git","fortune-mod","mc","abs","base-devel","inetutils","yajl","procps-ng",
        "scons","postgresql","libpqxx","boost","boost-libs", "libmemcached", "memcached", "json-c"

        ]

users = [
        "steven"
        ]

pacman_install = "pacman -S --noconfirm --needed %s"
add_user = "useradd -m -g users -G audio,lp,optical,storage,video,wheel,games,power,scanner -s /bin/zsh %s"

def pacman_check_installed(package):
    if run("pacman -Qs %s"%package):
        True
    else :
        False

def yaourt_install(package):
    if not pacman_check_installed(package):
        sudo("yaourt --noconfirm %s"%package)

def read_key_file(key_file):
    key_file = os.path.expanduser(key_file)
    if not key_file.endswith('pub'):
        raise RuntimeWarning('Trying to push non-public part of key pair')
    with open(key_file) as f:
        return f.read()

#@task
def push_key(user, key_file='~/.ssh/id_dsa.pub'):
    key_text = read_key_file(key_file)
    if not exists("/home/%s/.ssh/id_dsa"%user):
        sudo ("ssh-keygen -q -t dsa", user=user)
    authorized_keys ='/home/%s/.ssh/authorized_keys'%user
    append(authorized_keys , key_text)


def remote_deploy():
    try:
        run("pacman -Syu --noconfirm --force --ignore filesystem  && pacman -S filesystem --noconfirm --needed --force")
    except:
        run("pacman -Syu --noconfirm --force --ignore filesystem && rm -rf /var/run /var/lock && pacman -S filesystem --force")

    run(pacman_install % " ".join(arch_packages))
    run("abs")
    append("/etc/sudoers","%wheel ALL=(ALL) ALL")
    for user in users:
        if not exists("/home/%s/"%user):
            run(add_user % user)
            print "Enter Password for %s:"%user
            run("passwd %s" %user)
        with settings(sudo_prefix="%s -u %s"%(env.sudo_prefix,user)):
            push_key(user)
            with cd("/home/%s/"%user):
                try:
                    sudo("git status .")
                except:
                    sudo("git init .;git remote add origin git@github.com:jagguli/dotfiles.git;git pull origin master")
                sudo("git submodule update --init  --recursive")
                sudo("touch .zshrc.local")
                if not exists(".vim/.git/"):
                    if exists(".vim"):
                        sudo("rm -rf .vim")
                    sudo("git clone git@github.com:jagguli/vim-settings.git .vim")
                with cd (".vim"):
                    sudo("git submodule update --init  --recursive")
                sudo("ln -sf ~/.vim/vimrc .vimrc")
                sudo("mkdir -p abs")
                with cd ("abs"):
                    sudo("mkdir -p cower")
                    with cd ("cower"):
                        sudo("wget https://aur.archlinux.org/packages/co/cower-git/PKGBUILD")
                        sudo("makepkg -i")
                    with cd ("package-query"):
                        sudo("makepkg -i")
                    with cd ("yaourt"):
                        sudo("makepkg -i")
                yaourt_install("tmux-git")
                yaourt_install("soci-git")
                yaourt_install("mongrel2-git")
                sudo ("STLSOFT=/usr/include/stlsoft yaourt pantheios --noconfirm")
                yaourt_install("ctemplate")
                sudo("gem install compass")
                sudo("gem install compass-960-plugin")
                sudo("git config --global user.email \"steven@stevenjoseph.in\"")
                sudo("git config --global user.name \"Steven Joseph\"")


def setup_postgres():
    run("rc.d start postgresql")
    sudo("createuser -s -U postgres steven")


def create_github_token():
    global github_token
    github_user = "jagguli"
    val = local("""curl -u '%s' -d '{"scopes":["repo"],"note":"Help example"}' \
                https://api.github.com/authorizations"""%github_user, capture=True)
    import json
    val= json.loads(val)
    github_token = val['token']
    return github_token

def github_deploy_ssh_keys():
    user = "steven"
    github_token = ""
    #github_token = create_github_token()
    get("/home/%s/.ssh/id_dsa.pub"%user,"~/remote_sshkey.pub")
    key = read_key_file("~/remote_sshkey.pub").strip()
    #local("""curl -H "Authorization: token %s" https://api.github.com/repos/jagguli/dotfiles""" % github_token)
    for repo in ["dotfiles","vim-settings"]:
        local("""curl -u '%(user)s' -d '{"title": "%(title)s","key": "%(key)s"}' https://api.github.com/repos/%(user)s/%(repo)s/keys"""% { "user":"jagguli", "token":github_token, "title":"steven@%s"%env['host_string'],"key":key, "repo":repo})

def deploy(local=False):
    if local:
        pass
    else:
        remote_deploy()

