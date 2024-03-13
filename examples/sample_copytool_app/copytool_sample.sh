echo "\n Creating a temporary file."
echo "File for upload" > my_file.dat

##Need HESTIA_SRC_DIR  ???
echo "\n Defining the path of the binary hestia."
export PWD=`pwd`
export HESTIA_SRC_DIR=$(PWD)/../../
export PATH=$HESTIA_SRC_DIR/build/bin:$PATH

##Clean Cache folder  ???
echo "\n Cleaning cache."
rm -r ~/.cache/hestia

##Assume Server initialised already ??
##yaml info ??

# Create an object with id 01
echo "\n Creating an object with id 01."
hestia object create 01

# Add data from my_file_in.dat to this object. By default it will go to the 'fastest' Storage Tier, tier 0:
echo "\n Adding the file to tier 0."
hestia object put_data 01 --file my_file.dat

# Copy the contents of the object to another Storage Tier, tier 1 and tier 2
echo "\n Copying the file from tier 0 to tier 1."
hestia object copy_data 01 --source 0 --target 1
echo "\n Copying the file from tier 0 to tier 2."
hestia object copy_data 01 --source 0 --target 2

# Move the contents of the object from tier 1 and tier 3
echo "\n Moving the file from tier 1 to tier 3."
hestia object move_data 01 --source 1 --target 3

# Copy the contents of the object from tier 3 and tier 1
echo "\n Copying the file from tier 3 to tier 1."
hestia object copy_data 01 --source 3 --target 1

echo "\n Removing the temporary file."
rm my_file.dat
