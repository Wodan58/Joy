for i in `cat files`
do
echo $i
diff -w $i ../../JOY/Joy/$i
done >t
