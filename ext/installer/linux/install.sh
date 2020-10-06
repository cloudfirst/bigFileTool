sudo mkdir -p /usr/local/bigfiletool/myrouter
sudo mkdir -p /usr/local/bigfiletool/client
sudo chown -R $USER:$USER  /usr/local/bigfiletool

sudo cp ./bigfiletool_dist/*  /usr/local/bigfiletool/

sudo apt-get install libxcb-xinerama0 libpng12-0
