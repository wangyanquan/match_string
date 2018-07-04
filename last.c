/*************************************************************************
	> File Name: last.c
	> Author: wangyanquan
	> Mail: 
	> Created Time: 2018年03月16日 星期五 10时33分15秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define swap(a, b) {\
    __typeof(a) __tmp = a;\
    a = b;\
    b= __tmp;\
}
typedef struct Node {   //节点
    unsigned char key;           
    int flag;           //字符转结束标记位
    int ind, cnt;       //孩子节点的位置与数量
    struct Node *child, *brother;    //孩子节点与兄弟节点
} Node;

typedef struct Pool {   //内存池
    Node **start;        //节点指针数组
    int length, size;   
} Pool;
Pool *init_pool(int size) {  //初始化内存池
    Pool *p = (Pool *)malloc(sizeof(Pool) * 1);
    p -> length = 0;
    p -> size = size;
    p -> start = (Node **)calloc(size, sizeof(Node*));
    return p;
}
Pool *expand_pool(Pool *p) {
    p -> start = (Node **) realloc(p -> start, (p -> size * sizeof(Node *)) + (100 * sizeof(Node *)));
    p -> size += 100;    //内存之不够了之后每次加100
    return p;
}
Node *get_node(Pool *p, unsigned char x) {  //制造节点
    if (p -> length == p -> size) {
        p = expand_pool(p);
    }
    Node *tem = (Node *)malloc(1 * sizeof(Node));//内存池并没有开辟空间,在这里开辟
    tem -> key = x;
    tem -> flag = 0;
   // tem -> ind = p -> length;
    tem -> ind = 0;
    tem -> cnt = 0;
    tem -> child = NULL;
    tem -> brother = NULL;
    p -> start[p -> length] = tem;  //内存池仅仅记录地址
    p -> length++;
    return tem;
}
void insert(Pool* pool, unsigned char *str) {    //十字插入
    Node *p = pool -> start[0];
    if (p == NULL) 
        printf ("need head\n");
    while(str[0]) {
        if (p -> child == NULL) {
            p -> child = get_node(pool, str[0]);
            p = p -> child;
        }
        else {
            if (str[0] < p -> child -> key) {
                Node *tem = p -> child;
                p -> child = get_node(pool, str[0]);
                p = p -> child;
                p -> brother = tem;
            }
            else if (str[0] == p -> child -> key) {
                p = p -> child;
            }
            else {
                p = p -> child;
                while (p -> brother != NULL && str[0] != p -> brother -> key) {
                    p = p -> brother;
                }   
                if (p -> brother == NULL) {
                    p -> brother = get_node(pool, str[0]);
                }
                p = p -> brother; 
            }
        }
        str++;
    }
    p -> flag = 1;
    return;
}
void bfs(Pool *pool) {      //将十字树横向连接起来，并填充cnt
    Node *root = pool -> start[0];
    if (root == NULL)
        printf ("bfs wrong! need head\n");
    Node *p = root;
    Node *q = NULL;
    p -> brother = p -> child;   //root先给定brother后才能走
    int count;
    while (p -> brother != NULL) {
        count = 0;                      //孩子数量
        if (p -> child != NULL) {    //有孩子，q下去
            q = p -> child;
            count++;
        }
        while (q -> brother != NULL) {   //横扫节点数量，并记录
            q = q -> brother;
            count++;
        }
        p -> cnt = count;          //填充孩子节点数量
        p = p -> brother;          //p走一步
        if (p -> child != NULL) {    //如果p孩子，连接在这里
            q -> brother = p -> child;
        }
    }
    return;
}
void swap_node(Pool *pool) {   //将节点按照层序遍历的顺序摆好位置，只是指针的位置，不是实际内存的位置
    Node *p = pool -> start[0];
    if (p == NULL) 
        printf ("swap_node wrong!\n");
    int current_ind = 1;
    for(int i = 0; i < pool -> length - 1; i++) {
      //  swap(pool -> start[current_ind], pool -> start[current_ind - 1] -> brother);//这个命令有点问题，其实用不到交换
        pool -> start[current_ind] = pool -> start[current_ind - 1] -> brother;  //按照前一个brother的顺序走。
        current_ind++;
    }
    return;
}
void set_ind(Pool *pool) {   //填充ind，要在上一个函数完成之后  (函数思想，等待)
    Node *p = pool -> start[0];
    if (p == NULL) 
        printf ("set_ind wrong!\n");
    int current_ind = 1;    //这个标记当前节点位置
    int father_ind = 0, cnt_cout = 0;  //父节点位置与孩子数量
    for(int i = 0; i < pool -> length - 1; i++) {
        if (cnt_cout == 0 && father_ind < pool -> length) {  //每当上一个父节点的孩子数量消耗完时
            pool -> start[father_ind] -> ind = current_ind;  //便迎来当前父节点的孩子的位置。
            cnt_cout = pool -> start[father_ind] -> cnt;    //用当前父节点的孩子数重新填充cnt_count.
            father_ind ++;                                  //新的父节点等待上一个父节点的孩子树消耗为0的那一刻
            while (father_ind < pool -> length && pool -> start[father_ind] -> cnt == 0) {
                pool -> start[father_ind] -> ind = 0;   //如果新的父节点没有孩子，一直找到有孩子节点的父节点
                father_ind++;                         //这里一定要注意这种情况
            }
        }
        cnt_cout--;           //每走一步孩子节点数减少
        current_ind++;        //当前位置增加
    }
    return;
}
void output_ind(Pool *pool) {  //按内存池下标输出
    int ind = 0;
    while (pool -> start[ind] != NULL) {
        printf ("node is %d, ind = %d, cnt = %d,flag = %d\n", pool -> start[ind] -> key, pool -> start[ind] -> ind, pool -> start[ind] -> cnt, pool -> start[ind] -> flag);
        ind++;
    }
    return;
}
void output_brother(Pool *pool) { //按brother关系输出，与上面其实一样
    Node *p = pool -> start[0];
    while (p != NULL) {
        printf ("node is %d, ind = %d, cnt = %d\n", p -> key, p -> ind, p -> cnt);
        p = p -> brother;
    }
    return;
}
Node * binary_search(Pool *pool, Node *head, unsigned char str) {  //二分查找内存池中head父节点的孩子
    int left = head -> ind, right = head -> ind + head -> cnt - 1;
    int mid;
//    printf ("left = %d, right = %d\n", left, right);
    while (left <= right) {
        mid = (left + right) >> 1;
        if (str > pool -> start[mid] -> key) {
            left = mid + 1;
        }
        else if (str < pool -> start[mid] -> key) {
            right = mid - 1;
        }
        else {
            return pool -> start[mid];
        }
    }
    return NULL;
}
int search(Pool *pool, unsigned char *str) {//字符串匹配， 有回溯
    Node *head = pool -> start[0];
    int ret = 0;
    int len = strlen((const char *)str);
    for (int i = 0; i < len; i++) {
        unsigned char *cur_str = str + i;  //回溯
        head = pool -> start[0];
        while (cur_str[0]) {
            head = binary_search(pool, head, cur_str[0]);
            if (head == NULL) {
                break;
            } 
            else if (head-> flag == 1) {
                ret++;
            }
            cur_str++;
        }
    }
    return ret;
}
void output_str(unsigned char *str) {
    while(str[0]) {
        printf ("%d ",str[0]);
        str++;
    }
    return;
}
void clear(Pool *pool) {
    for (int i = 0; i < pool -> length; i++) {
        free(pool -> start[i]);
    }
    free(pool -> start);
    free(pool);
    return;
}
int main() {
    Pool *pool = init_pool(5);
    Node *root = get_node(pool, 0);
    FILE *input = fopen("/home/quan/xiu/字典树项目/data/company.txt", "r");
    FILE *corpus = fopen("/home/quan/xiu/字典树项目/data/corpus1.txt", "r");
  //  FILE *output = fopen("/home/quan/xiu/字典树项目/data/output", "w+");
    unsigned char *str = (unsigned char *)calloc (100000, sizeof(char));
    unsigned char *tem = (unsigned char *)calloc (1000, sizeof(char));
/*    while (fgets((char *)str, 100000 * sizeof(char), input)) {
        sscanf((const char *)str, "%s", str);
        insert(pool, str);
        memset(str, 0, 100000 * sizeof(char));
    }  */
    while (fscanf(input,"%[^\n\t]\t", str) != EOF) {
        if (str[0] >= '0' && str[0] <= '9') continue;
        insert(pool, str);
        memset(str, 0, 1000);
    }
    bfs(pool);
    //output_ind(pool);
    swap_node(pool);
   // output_ind(pool);
    set_ind(pool);
    printf ("pool -> size = %d, pool -> length = %d\n", pool -> size, pool -> length);
    //  output_ind(pool);
    int count = 0;
    while (~fscanf(corpus, "%s", str)) {
        count += search(pool, str);
    }  
    printf("finally, count = %d\n", count);
  /*  while (~scanf("%s", str)) { 
        count = search(pool, str);
        printf ("str = %s, count = %d\n", str, count);
        memset(str, 0, 100000 * sizeof(char));
    }  */ 
    fclose(input);
    fclose(corpus);
    free(str);
    free(tem);
    free(root);
    free(pool);
    return 0;
}
