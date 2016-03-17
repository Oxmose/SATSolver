for f in `ls`
do
    echo $f
    mv $f ".$f"
done
