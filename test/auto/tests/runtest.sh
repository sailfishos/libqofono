#!/bin/sh -e

systemctl --user restart phonesim
sleep 15
exec "/opt/tests/libqofono-qt5/${1?Test executable expected}"
