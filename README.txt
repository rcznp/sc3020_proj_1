1)run test_disk to load games.txt
g++ -std=c++11 test_disk.cc disk.cc record_block.cc bptree.cc -o test_disk
./test_disk
=======================
can test loading of b+ tree with test_load_tree.cc


2)then run the code in tasks folder(for task 3 need run from out side tasks folder dk why)