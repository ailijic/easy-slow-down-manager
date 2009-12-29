# Contributor: George Kibardin <george-kibardin at yandex dot ru>
pkgname=easy-slow-down-manager
pkgver=0.2
pkgrel=1
pkgdesc="A fan control kernel module for Samsung laptops"
arch=('i686' 'x86_64')
url="http://code.google.com/p/easy-slow-down-manager"
license=('GPL')
depends=('kernel26')
source=(easy-slow-down-manager.tar.gz)
md5sums=('91293b5bf62dfc74ae86c9186ec49a92')

build() {
  cd "$srcdir"
#compile the module
  make -C /usr/src/linux-`uname -r` M=`pwd` modules || return 1
#install the module in the pkgdir
  install -D -m 644 easy_slow_down_manager.ko $pkgdir/lib/modules/`uname -r`/kernel/drivers/misc/easy_slow_down_manager.ko || return 1
  install -D -m 644 README $pkgdir/usr/share/doc/easy-slow-down-manager/README || return 1
}

