
# sign framework
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtPrintSupport.framework/QtPrintSupport
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtGui.framework/QtGui
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtDBus.framework/QtDBus
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtQuick.framework/QtQuick
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtCore.framework/QtCore
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtQml.framework/QtQml
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtVirtualKeyboard.framework/QtVirtualKeyboard
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtWidgets.framework/QtWidgets
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtNetwork.framework/QtNetwork
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtSvg.framework/QtSvg
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/Frameworks//QtQmlModels.framework/QtQmlModels

# sign plugin
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//platforminputcontexts/libqtvirtualkeyboardplugin.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//platforms/libqcocoa.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//printsupport/libcocoaprintersupport.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//styles/libqmacstyle.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//bearer/libqgenericbearer.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//iconengines/libqsvgicon.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqgif.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqwbmp.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqwebp.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqico.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqmacheif.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqjpeg.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqtiff.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqicns.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqtga.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//imageformats/libqmacjp2.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//virtualkeyboard/libqtvirtualkeyboard_thai.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//virtualkeyboard/libqtvirtualkeyboard_openwnn.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//virtualkeyboard/libqtvirtualkeyboard_hangul.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//virtualkeyboard/libqtvirtualkeyboard_pinyin.dylib
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)" ./bigfiletool.app/Contents/PlugIns//virtualkeyboard/libqtvirtualkeyboard_tcime.dylib

# sign App
codesign -f -s "Developer ID Application: shanghai oxfold network Co., Ltd (A2P542M34H)"  ./bigfiletool.app

# 
# macdeployqt APP_NAME.app -codesign="Developer ID Application: YOUR NAME (ID_NUMBERS)"
# codesign --force --verify --verbose --sign "Developer ID Application: YOUR NAME (ID_NUMBERS)" APP_NAME.app


# sign install pkg
# productsign --sign "Developer ID Installer: shanghai oxfold network Co., Ltd (A2P542M34H)" bigfiletool.pkg bigfiletool-signed.pkg