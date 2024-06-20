# cool-retro-term

### Install the necessary packages: 
#### CM3 
```
sudo apt update
sudo apt install build-essential qmlscene qt5-qmake qt5-default qtdeclarative5-dev qml-module-qtquick-controls qml-module-qtgraphicaleffects qml-module-qtquick-dialogs qml-module-qtquick-localstorage qml-module-qtquick-window2 qml-module-qt-labs-settings qml-module-qt-labs-folderlistmodel
```
#### ubuntu (A06)
```
sudo apt update
sudo apt install build-essential qmlscene qt5-qmake qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools  qtdeclarative5-dev qml-module-qtquick-controls qml-module-qtgraphicaleffects qml-module-qtquick-dialogs qml-module-qtquick-localstorage qml-module-qtquick-window2 qml-module-qt-labs-settings qml-module-qt-labs-folderlistmodel
```
### Compile (it takes about 30 minutes):
```
cd ~
git clone --recursive https://github.com/Swordfish90/cool-retro-term.git
cd cool-retro-term
git reset --hard dac2b4
qmake && make
cp -r qmltermwidget/src/qmldir qmltermwidget/lib/kb-layouts qmltermwidget/lib/color-schemes qmltermwidget/src/QMLTermScrollbar.qml qmltermwidget/QMLTermWidget
```

### Run

`./cool-retro-term --fullscreen`