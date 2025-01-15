#include "BinarySearchTree.hpp"
#include "TraverseTree.hpp"

#define PRINT_BST(root)\
    TEST_TRAVERSE("先序遍历", root, TraversePrOrderR)\
    TEST_TRAVERSE("中序遍历", root, TraverseInOrderR)

#if 0
int main() {
    try {
        std::cout << "新建二叉查找树 ...\n";
        AdelsonVelskyLandis<int> tree1{ 11, 7, 17, 5, 9, 14, 19, 8, 12, 18, 27, 23 };
        PRINT_BST(tree1.root);
        std::cout << "二叉查找树删除结点 ...\n";
        tree1.Delete(tree1.Search(5));
        PRINT_BST(tree1.root);
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
    }

    return 0;
}
#endif
