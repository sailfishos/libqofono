#!/bin/sh -e

systemctl --user restart phonesim
sleep 15
exec /usr/sbin/run-blts-root /bin/su $USER -g sailfish-radio \
        -c "/opt/tests/libqofono-qt5/${1?Test executable expected}"
