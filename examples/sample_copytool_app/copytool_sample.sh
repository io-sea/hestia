echo "\n1. Creating a temporary file."
echo "File for upload" > my_file.dat

# Create an object with id 01
echo "\n2. Creating an object with id 01.\n"
hestia object create 01

# Add data from my_file_in.dat to this object. By default it will go to the 'fastest' Storage Tier, tier 0:
echo "\n3. Adding the file to tier 0.\n"
hestia object put_data 01 --file my_file.dat

# Copy the contents of the object to another Storage Tier, tier 1 
echo "\n4. Copying the file from tier 0 to tier 1.\n"
hestia object copy_data 01 --source 0 --target 1

# Move the contents of the object from tier 1 to tier 2
echo "\n5. Moving the file from tier 1 to tier 2.\n"
hestia object move_data 01 --source 1 --target 2

# Copy the contents of the object from tier 2 to tier 1
echo "\n6. Copying the file from tier 2 to tier 1.\n"
hestia object copy_data 01 --source 2 --target 1

# Get and compare the contents of the files in tier 0 and tier 1
echo "\n7. Getting and comparing the contents of the files in tier 0 and tier 1.\n"
hestia object get_data 01 --tier 0 --file my_file0_out.dat 
hestia object get_data 01 --tier 1 --file my_file1_out.dat 
if diff "my_file0_out.dat" "my_file1_out.dat" &> /dev/null ; then
    echo "\n- Files on tier 0 and tier 1 are the same.\n"
else
    echo "\n- Files on tier 0 and tier 1 differ.\n"
fi

echo "8. Removing the temporary files.\n"
rm my_file.dat my_file0_out.dat my_file1_out.dat
