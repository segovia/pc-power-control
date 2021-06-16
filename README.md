HTML files for deploying on `/home/pi/sites/pc-power-control-web/` is contained in `/html`.

The rest is an ESP32 project.


To auto-deploy web:

while inotifywait -r -e modify,create,delete,move /home/gustavo/workspace/arduino/pc-power-control/html/; do rsync -a /home/gustavo/workspace/arduino/pc-power-control/html/ pi@mediapi.local:/home/pi/sites/pc-power-control-web/; done