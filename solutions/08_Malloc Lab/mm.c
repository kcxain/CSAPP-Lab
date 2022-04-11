/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "haha",
    /* First member's full name */
    "deconx",
    /* First member's email address */
    "deconx@qq.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 头部/脚部的大小 */
#define WSIZE 4
/* 双字 */
#define DSIZE 8

/* 扩展堆时的默认大小 */
#define CHUNKSIZE (1 << 12)

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* 设置头部和脚部的值, 块大小+分配位 */
#define PACK(size, alloc) ((size) | (alloc))

/* 读写指针p的位置 */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) ((*(unsigned int *)(p)) = (val))

/* 从头部或脚部获取大小或分配位 */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)


/* 给定序号，找到链表头节点位置 */
#define GET_HEAD(num) ((unsigned int *)(long)(GET(heap_list + WSIZE * num)))
/* 给定bp,找到前驱和后继 */
#define GET_PRE(bp) ((unsigned int *)(long)(GET(bp)))
#define GET_SUC(bp) ((unsigned int *)(long)(GET((unsigned int *)bp + 1)))

/* 读地址存的指针 */
#define GET_PTR(p) ((unsigned int *)(long)(GET(p)))

/* 给定有效载荷指针, 找到头部和脚部 */
#define HDRP(bp) ((char*)(bp) - WSIZE)
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* 给定有效载荷指针, 找到前一块或下一块 */
#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))

#define CLASS_SIZE 20
/* 总是指向序言块的第二块 */
static char *heap_list;

/************************/
static void *extend_heap(size_t words);     //扩展堆
static void *coalesce(void *bp);            //合并空闲块
static void *find_fit(size_t asize);        //找到匹配的块
static void place(void *bp, size_t asize);  //分割空闲块
static void delete(void *bp);               //从相应链表中删除块
static void insert(void *bp);               //在对应链表中插入块
static int search(size_t size);             //根据块大小, 找到头节点位置
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* 申请四个字节空间 */
    if((heap_list = mem_sbrk((4+CLASS_SIZE)*WSIZE)) == (void *)-1)
        return -1;
    /* 初始化20个大小类头指针 */
    for(int i = 0; i < CLASS_SIZE; i++){
        PUT(heap_list + i*WSIZE, NULL);
    }
    /* 对齐 */
    PUT(heap_list + CLASS_SIZE * WSIZE, 0);
    /* 
     * 序言块和结尾块均设置为已分配, 方便考虑边界情况
     */
    PUT(heap_list + ((1 + CLASS_SIZE)*WSIZE), PACK(DSIZE, 1));     /* 填充序言块 */
    PUT(heap_list + ((2 + CLASS_SIZE)*WSIZE), PACK(DSIZE, 1));     /* 填充序言块 */
    PUT(heap_list + ((3 + CLASS_SIZE)*WSIZE), PACK(0, 1));         /* 结尾块 */


    /* 扩展空闲空间 */
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}


/*
 * 扩展heap, 传入的是字节数
*/
void *extend_heap(size_t words)
{
    /* bp总是指向有效载荷 */
    char *bp;
    size_t size;
    /* 根据传入字节数奇偶, 考虑对齐 */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;

    /* 分配 */
    if((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    
    /* 设置头部和脚部 */
    PUT(HDRP(bp), PACK(size, 0));           /* 空闲块头 */
    PUT(FTRP(bp), PACK(size, 0));           /* 空闲块脚 */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));   /* 片的新结尾块 */

    /* 判断相邻块是否是空闲块, 进行合并 */
    return coalesce(bp);
}


/*
 * 合并空闲块
*/
void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));     /* 前一块大小 */
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));     /* 后一块大小 */
    size_t size = GET_SIZE(HDRP(bp));                       /* 当前块大小 */

    /*
     * 四种情况：前后都不空, 前不空后空, 前空后不空, 前后都空
     */
    /* 前后都不空 */
    if(prev_alloc && next_alloc){
        insert(bp);
        return bp;
    }
    /* 前不空后空 */
    else if(prev_alloc && !next_alloc){
        /* 将后面的块从其链表中删除 */
        delete(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));  //增加当前块大小
        PUT(HDRP(bp), PACK(size, 0));           //先修改头
        PUT(FTRP(bp), PACK(size, 0));           //根据头部中的大小来定位尾部
    }
    /* 前空后不空 */
    else if(!prev_alloc && next_alloc){
        /* 将其前面的快从链表中删除 */
        delete(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));  //增加当前块大小
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);                     //注意指针要变
    }
    /* 都空 */
    else{
        /* 将前后两个块都从其链表中删除 */
        delete(NEXT_BLKP(bp));
        delete(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));  //增加当前块大小
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    /* 空闲块准备好后,将其插入合适位置 */
    insert(bp);
    return bp;
}
/*
 *  插入块, 将块插到表头
 */
void insert(void *bp)
{
    /* 块大小 */
    size_t size = GET_SIZE(HDRP(bp));
    /* 根据块大小找到头节点位置 */
    int num = search(size);
    /* 空的，直接放 */
    if(GET_HEAD(num) == NULL){
        PUT(heap_list + WSIZE * num, bp);
        /* 前驱 */
        PUT(bp, NULL);
		/* 后继 */
        PUT((unsigned int *)bp + 1, NULL);
	} else {
        /* bp的后继放第一个节点 */
		PUT((unsigned int *)bp + 1, GET_HEAD(num));
		/* 第一个节点的前驱放bp */
        PUT(GET_HEAD(num), bp);
        /* bp的前驱为空 */  	
		PUT(bp, NULL);
        /* 头节点放bp */
		PUT(heap_list + WSIZE * num, bp);
	}
}
/*
 *  删除块,清理指针
 */
void delete(void *bp)
{
    /* 块大小 */
    size_t size = GET_SIZE(HDRP(bp));
    /* 根据块大小找到头节点位置 */
    int num = search(size);
    /* 
     * 唯一节点,后继为null,前驱为null 
     * 头节点设为null
     */
	if (GET_PRE(bp) == NULL && GET_SUC(bp) == NULL) { 
		PUT(heap_list + WSIZE * num, NULL);
	} 
    /* 
     * 最后一个节点 
     * 前驱的后继设为null
     */
    else if (GET_PRE(bp) != NULL && GET_SUC(bp) == NULL) {
		PUT(GET_PRE(bp) + 1, NULL);
	} 
    /* 
     * 第一个结点 
     * 头节点设为bp的后继
     */
    else if (GET_SUC(bp) != NULL && GET_PRE(bp) == NULL){
		PUT(heap_list + WSIZE * num, GET_SUC(bp));
		PUT(GET_SUC(bp), NULL);
	}
    /* 
     * 中间结点 
     * 前驱的后继设为后继
     * 后继的前驱设为前驱
     */
    else if (GET_SUC(bp) != NULL && GET_PRE(bp) != NULL) {
		PUT(GET_PRE(bp) + 1, GET_SUC(bp));
		PUT(GET_SUC(bp), GET_PRE(bp));
	}
}
/* 
 * search - 找到块大小对应的等价类的序号
 */
int search(size_t size)
{
    int i;
    for(i = 4; i <=22; i++){
        if(size <= (1 << i))
            return i-4;
    }
    return i-4;
    // if (size <= (1 << 4)) {
	// 	return 0;
	// } else if (size <= (1 << 5)) {
	// 	return 1;
	// } else if (size <= (1 << 6)) {
	// 	return 2;
	// } else if (size <= (1 << 7)) {
	// 	return 3;
	// } else if (size <= (1 << 8)) {
	// 	return 4;
	// } else if (size <= (1 << 9)) {
	// 	return 5;
	// } else if (size <= (1 << 10)) {
	// 	return 6;
	// } else if (size <= (1 << 11)) {
	// 	return 7;
	// } else if (size <= (1 << 12)) {
	// 	return 8;
	// } else if (size <= (1 << 13)) {
	// 	return 9;
	// } else if (size <= (1 << 14)) {
	// 	return 10;
	// } else if (size <= (1 << 15)) {
	// 	return 11;
	// } else if (size <= (1 << 16)) {
	// 	return 12;
	// } else if (size <= (1 << 17)) {
	// 	return 13;
	// } else if (size <= (1 << 18)) {
	// 	return 14;
	// } else if (size <= (1 << 19)) {
	// 	return 15;
	// } else if (size <= (1 << 20)) {
	// 	return 16;
	// } else if (size <= (1 << 21)){
	// 	return 17;
	// } else if (size <= (1 << 22)){
    //     return 18;
    // } else {
    //     return 19;
    // }
}
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    // int newsize = ALIGN(size + SIZE_T_SIZE); //对齐后的大小
    // void *p = mem_sbrk(newsize);
    // if (p == (void *)-1)
	//     return NULL;
    // else {
    //     *(size_t *)p = size;
    //     return (void *)((char *)p + SIZE_T_SIZE);
    // }
    size_t asize;
    size_t extendsize;
    char *bp;
    if(size == 0)
        return NULL;
    if(size <= DSIZE)
        asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
    /* 寻找合适的空闲块 */
    if((bp = find_fit(asize)) != NULL){
        place(bp, asize);
        return bp;
    }
    /* 找不到则扩展堆 */
    extendsize = MAX(asize, CHUNKSIZE);
    if((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;

}

/*
 * 适配
 */
void *find_fit(size_t asize)
{
    int num = search(asize);
    unsigned int* bp;
    /* 如果找不到合适的块，那么就搜索下一个更大的大小类 */
    while(num < CLASS_SIZE) {
        bp = GET_HEAD(num);
        /* 不为空则寻找 */
        while(bp) {
            if(GET_SIZE(HDRP(bp)) >= asize){
                return (void *)bp;
            }
            /* 用后继找下一块 */
            bp = GET_SUC(bp);
        }
        /* 找不到则进入下一个大小类 */
        num++;
    }
    return NULL;
}

/*
 * 分离空闲块
 */
void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    
    /* 块已分配，从空闲链表中删除 */
    delete(bp);
    if((csize - asize) >= 2*DSIZE) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        /* bp指向空闲块 */
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
        /* 加入分离出来的空闲块 */
        insert(bp);
    }
    /* 设置为填充 */
    else{
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}
/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    if(ptr==0)
        return;
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    /* size可能为0,则mm_malloc返回NULL */
    void *newptr;
    size_t copysize;
    
    if((newptr = mm_malloc(size))==NULL)
        return 0;
    copysize = GET_SIZE(HDRP(ptr));
    if(size < copysize)
        copysize = size;
    memcpy(newptr, ptr, copysize);
    mm_free(ptr);
    return newptr;
}