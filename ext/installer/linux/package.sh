# package the necessary files into a destination directory
rm -fr bigfiletool_dist || true
mkdir bigfiletool_dist

# cp bigfiletoo executable file
cp ../../../3rd/http/bin/linux-x86_64/*        ./bigfiletool_dist/
cp ../../../3rd/oxfold/lib/linux-x86_64/*.so  ./bigfiletool_dist/
