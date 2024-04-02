#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_N 10000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define EPSILON 0.00000001 //ε 小さい正の値
#define SWAP(a,b){int temp; temp=(a); (a)=(b); (b)=temp; }

struct point {
  int x;
  int y;
};

struct Node {
  int index;
  struct Node* next;
  struct Node* prev;
};

struct Node* createNode(int index) {
  struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
  if (newNode == NULL) {
    fprintf(stderr, "メモリの確保エラー.\n");
    exit(EXIT_FAILURE);
  }
  newNode->index = index;
  newNode->next = NULL;
  newNode->prev = NULL;
  return newNode;
}

void freeTour(struct Node* tourHead) {
  struct Node* current = tourHead;
  while (current != NULL) {
    struct Node* next = current->next;
    free(current);
    current = next;
  }
}

void insertAfter(struct Node* prevNode, int newIndex) {
  if (prevNode == NULL) {
    fprintf(stderr, "エラー: 指定されたノードが存在しません.\n");
    exit(EXIT_FAILURE);
  }

  struct Node* newNode = createNode(newIndex);
  newNode->next = prevNode->next;
  newNode->prev = prevNode;
  if (prevNode->next != NULL) {
    prevNode->next->prev = newNode;
  }
  prevNode->next = newNode;
}

void deleteNode(struct Node* currentNode) {
  if (currentNode == NULL) {
    fprintf(stderr, "エラー: 指定されたノードが存在しません.\n");
    exit(EXIT_FAILURE);
  }

  if (currentNode->prev != NULL) {
    currentNode->prev->next = currentNode->next;
  }
  if (currentNode->next != NULL) {
    currentNode->next->prev = currentNode->prev;
  }

  free(currentNode);
}

// リストの要素を配列に移す関数
void listToArray(struct Node* tourHead, int tour[MAX_N]) {
    struct Node* current = tourHead;
    int i = 0;

    while (current != NULL) {
        tour[i++] = current->index;
        current = current->next;
    }
}

// 配列の要素をリストに移す関数
struct Node* arrayToList(int arr[MAX_N], int n) {
    if (n <= 0) {
        fprintf(stderr, "エラー: 無効な配列長です。\n");
        exit(EXIT_FAILURE);
    }

    struct Node* head = createNode(arr[0]);
    struct Node* current = head;

    for (int i = 1; i < n; i++) {
        struct Node* newNode = createNode(arr[i]);
        current->next = newNode;
        newNode->prev = current;
        current = newNode;
    }

    return head;
}

void write_tourlist_data(char *filename, struct Node* tourHead) {
  FILE *fp;

  if ((fp = fopen(filename, "wt")) == NULL) {
    fprintf(stderr, "エラー: ファイル %s のオープンに失敗しました.\n", filename);
    exit(EXIT_FAILURE);
  }

  struct Node* current = tourHead;
  int n = 0;
  while (current != NULL) {
    n++;
    current = current->next;
  }

  fprintf(fp, "%d\n", n);

  current = tourHead;
  while (current != NULL) {
    fprintf(fp, "%d ", current->index);
    current = current->next;
  }
  fprintf(fp, "\n");

  fclose(fp);
}

void write_tourarray_data(char *filename, int n, int tour[MAX_N]) {
  FILE *fp;
  int i;

  if ((fp = fopen(filename, "wt")) == NULL) {
    fprintf(stderr, "Error: File %s open failed.\n", filename);
    exit(EXIT_FAILURE);
  }
  fprintf(fp, "%d\n", n);
  for (i = 0; i < n; i++) {
    fprintf(fp, "%d ", tour[i]);
  }
  fprintf(fp, "\n");
  fclose(fp);
}

double dist(struct point p, struct point q) {
  return sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y) * (p.y - q.y));
}

double tour_length_list(struct point p[MAX_N], struct Node* tourHead) {
  struct Node* current = tourHead;
  struct Node* tail;
  double sum = 0.0;

  while (current != NULL) {
    struct Node* next = current->next;
    if (next != NULL) {
      sum += dist(p[current->index], p[next->index]);
      tail = next;
    }
    current = next;
  }

  sum += dist(p[tourHead->index], p[tail->index]);

  return sum;
}

double tour_length_array(struct point p[MAX_N], int n, int tour[MAX_N]) {
  int i;
  double sum = 0.0;
  for (i = 0; i < n; i++)
    sum += dist(p[tour[i]], p[tour[(i + 1) % n]]);
  return sum;
}

void read_tsp_data(char *filename, struct point p[MAX_N], int *np, int prec[MAX_N], int *mp) {
  FILE *fp;
  char buff[500];
  int i;

  if ((fp = fopen(filename, "rt")) == NULL) {
    fprintf(stderr, "Error: File %s open failed.\n", filename);
    exit(EXIT_FAILURE);
  }

  while ((fgets(buff, sizeof(buff), fp) != NULL) && (strncmp("PRECEDENCE_CONSTRAINTS:", buff, 23) != 0)) ;
  if (strncmp("PRECEDENCE_CONSTRAINTS:", buff, 23) == 0) {
    sscanf(buff + 24, "%d", mp);
    for (i = 0; i < *mp; i++)
      fscanf(fp, "%d ", &prec[i]);
  } else {
    fprintf(stderr, "Error: There is no precedence constraint in file %s.\n", filename);
    exit(EXIT_FAILURE);
  }

  while ((fgets(buff, sizeof(buff), fp) != NULL) && (strncmp("DIMENSION", buff, 9) != 0)) ;
  sscanf(buff, "DIMENSION: %d", np);

  while ((fgets(buff, sizeof(buff), fp) != NULL) && (strncmp("NODE_COORD_SECTION", buff, 18) != 0)) ;

  for (i = 0; i < *np; i++) {
    if (fgets(buff, sizeof(buff), fp) != NULL)
      sscanf(buff, "%*d %d %d", &(p[i].x), &(p[i].y));
  }

  fclose(fp);
}

void write_tour_data(char *filename, int n, int tour[MAX_N]) {
  FILE *fp;
  int i;

  if ((fp = fopen(filename, "wt")) == NULL) {
    fprintf(stderr, "Error: File %s open failed.\n", filename);
    exit(EXIT_FAILURE);
  }
  fprintf(fp, "%d\n", n);
  for (i = 0; i < n; i++) {
    fprintf(fp, "%d ", tour[i]);
  }
  fprintf(fp, "\n");
  fclose(fp);
}

void insert(int tour[MAX_N], int *len, int k, int value) {
  int i;

  if (k < 0 || k > *len || *len >= MAX_N) {
    printf("挿入エラー: 範囲外\n");
    return;
  }

  for (i = *len; i > k; i--) {
    tour[i] = tour[i - 1];
  }
  tour[k] = value;
  (*len)++;
}

void ci(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]){
  printf("ciの実行\n");
  FILE *fp;
  int i,j,a,b,c,r;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
  int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
  double dist1,dist2, min_dist;
  int min_i,min_j,min_r;
  int sbtlen=0;

  for(a=0;a<n;a++) visited[a]=0; // 最初は全都市は未訪問

  sbtlen=0;
  for(r=0; r<m; r++){
    sbtlen++;
    tour[r] = prec[r]; 
    visited[prec[r]] = 1;
  }

  while(sbtlen<n) {
    min_dist=INF;
    for(r=0;r<n;r++) {
        if(!visited[r]) {
	        d[r]=INF;
	        for(i=0;i<sbtlen;i++) {
	            a=tour[i];
	            j=(i+1)%sbtlen; // j== i+1 ただし i=sbtlen-1 のときは, j==0
	            b=tour[j];
	            if (dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b])<d[r]) {
	                nearest[r]=i;
	                d[r]=dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b]);
	            }
	        }
	        if (d[r]<min_dist) {
	            min_dist = d[r];
	            min_r = r;
	        }
        }
    }
    r=min_r;
    i=nearest[r];
    j=(i+1)%sbtlen;

    insert(tour,&sbtlen,j,r); 
    visited[r]=1;
  }
}

void re_ci(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]){
  //printf("--re_ciの実行--");
  FILE *fp;
  int i,j,a,b,c,r=0;
  int fig=n/10;
  int top10[MAX_N],min_length=0;
  int topcity[2][MAX_N],tour_tmp[MAX_N],tour_tmp2[MAX_N];
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
  int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
  double dist1,dist2, min_dist;
  int min_i,min_j,min_r;
  int sbtlen=0;

  if(fig<100)fig=100;

  for(int i=0;i<fig;i++)top10[i]=0;
  for(int i=0;i<n;i++)tour_tmp2[i]=tour[i];

  for(a=0;a<n;a++) visited[a]=0; // 最初は全都市は未訪問

  //長い辺fig本を探す。
  for(i=0;i<n-1;i+=2){
      if(min_length<dist(p[tour[i]],p[tour[i+1]])){
        topcity[0][r]=i;
        topcity[1][r]=i+1;
        top10[r]=dist(p[tour[i]],p[tour[i+1]]);
        for(int kk=0;kk<m;kk++){
          if(prec[kk] == tour[i])topcity[0][r]=-1;
          if(prec[kk] == tour[i+1])topcity[1][r]=-1;
        }
        min_length=top10[r];
        for(int l=0;l<fig;l++){
          if(top10[l]<min_length){
            min_length=top10[l];
            r=l;
            //printf("%d ",r);
          }
        }
      }
      
    }
  

  sbtlen=0;
  for(r=0; r<n; r++){
    int improvement=0;
    for(i=0;i<2;i++)
     for(j=0;j<fig;j++){
       if(r==topcity[i][j]){
        improvement=1;
       }
     }
    if(improvement == 0){
      tour_tmp[sbtlen]=tour[r];
      visited[tour[r]]=1;
      sbtlen++;
    }
  }
  //printf("%d\n",sbtlen);

  for(int k=0;k<sbtlen;k++)tour[k]=tour_tmp[k];


  while(sbtlen<n) {
    min_dist=INF;
    for(int l=0;l<2;l++) {
        for(int k=0;k<fig;k++) {
            r=tour_tmp2[topcity[l][k]];
            if(topcity[l][k]<0||visited[r]==1)continue;
            //printf("%d ",topcity[l][k]);
	        d[r]=INF;
	        for(i=0;i<sbtlen;i++) {
	            a=tour[i];
	            j=(i+1)%sbtlen; // j== i+1 ただし i=sbtlen-1 のときは, j==0
	            b=tour[j];
	            if (dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b])<d[r]) {
	                nearest[r]=i;
	                d[r]=dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b]);
	            }
	        }
	        if (d[r]<min_dist) {
	            min_dist = d[r];
	            min_r = r;
	        }
        }
    }
    
    r=min_r;
    i=nearest[r];
    j=(i+1)%sbtlen;

    insert(tour,&sbtlen,j,r); 
    visited[r]=1;
  }
}

void re_rnd_1_ci(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]){
  //printf("re_rand_ciの実行\n");
  FILE *fp;
  int i,j,a,b,c,r;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
  int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
  int tour_tmp[MAX_N];
  double dist1,dist2, min_dist;
  int min_i,min_j,min_r;
  int sbtlen=0,random_i;
  int denode[MAX_N];
  int prec_i[MAX_N];


  for(int l=0;5>l;l++){
    //printf("%d ------\n",l);
    for(int k=0;k<n;k++)prec_i[k]=0;
    for(int k=0;k<m;k++)prec_i[prec[k]]=1;
    for(int k=0;k<n;k++)denode[k]=0;
    for(int k=0;k<n;k++)visited[k]=0;
    sbtlen=0;
    random_i=rand()%n;
    for(int k=0;k<n/50;k++){
        while(prec_i[tour[random_i]]){
            random_i++;
        }
        denode[random_i]=1;
        //printf("%d ",random_i);
        random_i++;
    }//printf("\n");
    
   for(r=0; r<n; r++){
    if(denode[r]==1)continue;

    tour_tmp[sbtlen] = tour[r];
    visited[tour[r]] = 1;
    sbtlen++;
   }
   //printf("%d\n",sbtlen);
   for(int l=0;l<sbtlen;l++)tour[l]=tour_tmp[l];

   while(sbtlen<n) {
    min_dist=INF;
    //printf("%d %d %d\n",tour[sbtlen-2],tour[sbtlen-1],sbtlen);
    for(r=0;r<n;r++) {
        if(!visited[r]) {
	        d[r]=INF;
	        for(i=0;i<sbtlen;i++) {
	            a=tour[i];
	            j=(i+1)%sbtlen; // j== i+1 ただし i=sbtlen-1 のときは, j==0
	            b=tour[j];
	            if (dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b])<d[r]) {
	                nearest[r]=i;
	                d[r]=dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b]);
	            }
	        }
	        if (d[r]<min_dist) {
	            min_dist = d[r];
	            min_r = r;
	        }
        }
     }
     r=min_r;
     i=nearest[r];
     j=(i+1)%sbtlen;
     //printf("%d\n",r);
     insert(tour,&sbtlen,j,r); 
     visited[r]=1;

    }
   }
}

void re_rnd_ci(struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]){
  //printf("re_rand_ciの実行\n");
  FILE *fp;
  int i,j,a,b,c,r;
  int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
  double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
  int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
  int tour_tmp[MAX_N];
  double dist1,dist2, min_dist;
  int min_i,min_j,min_r;
  int sbtlen=0,random_i;
  int denode[MAX_N];
  int prec_i[MAX_N];


  for(int l=0;5>l;l++){
    //printf("%d ------\n",l);
    for(int k=0;k<n;k++)prec_i[k]=0;
    for(int k=0;k<m;k++)prec_i[prec[k]]=1;
    for(int k=0;k<n;k++)denode[k]=0;
    for(int k=0;k<n;k++)visited[k]=0;
    sbtlen=0;
    random_i=rand()%n;
    for(int k=0;k<n/50;k++){
        while(prec_i[tour[random_i]]){
            random_i=rand()%n;
        }
        denode[random_i]=1;
        //printf("%d ",random_i);
        
    }//printf("\n");
    
   for(r=0; r<n; r++){
    if(denode[r]==1)continue;

    tour_tmp[sbtlen] = tour[r];
    visited[tour[r]] = 1;
    sbtlen++;
   }
   //printf("%d\n",sbtlen);
   for(int l=0;l<sbtlen;l++)tour[l]=tour_tmp[l];

   while(sbtlen<n) {
    min_dist=INF;
    //printf("%d %d %d\n",tour[sbtlen-2],tour[sbtlen-1],sbtlen);
    for(r=0;r<n;r++) {
        if(!visited[r]) {
	        d[r]=INF;
	        for(i=0;i<sbtlen;i++) {
	            a=tour[i];
	            j=(i+1)%sbtlen; // j== i+1 ただし i=sbtlen-1 のときは, j==0
	            b=tour[j];
	            if (dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b])<d[r]) {
	                nearest[r]=i;
	                d[r]=dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b]);
	            }
	        }
	        if (d[r]<min_dist) {
	            min_dist = d[r];
	            min_r = r;
	        }
        }
     }
     r=min_r;
     i=nearest[r];
     j=(i+1)%sbtlen;
     //printf("%d\n",r);
     insert(tour,&sbtlen,j,r); 
     visited[r]=1;

    }
   }
}


// or-opt-1近傍法による改善
void or_opt_1(struct point p[MAX_N], int n, struct Node* tourHead, int m, int prec[MAX_N]) {
  // printf("or-pot-1近傍法の実行\n");
  int improvement = 1;
  int k = 1;
  int label[MAX_N];

  for(int a=0;a<n;a++) label[a]=0;
  for(int a=0;a<m;a++) label[prec[a]]=1;


  while(improvement) {
    int i = 0;
    improvement = 0;
    struct Node* a = tourHead->next->next;
    while(a->next->next != NULL){
      if(label[a->index] == 0){
        struct Node* b = tourHead->next;
        struct Node* c = b->next;
        while(c->next != NULL){
            if(b != a && c != a){
                double before = dist(p[a->prev->index],p[a->index]) + dist(p[a->index],p[a->next->index]) + dist(p[b->index],p[c->index]);
                double after = dist(p[a->prev->index],p[a->next->index]) + dist(p[b->index],p[a->index]) + dist(p[a->index],p[c->index]);

                struct Node* a_copy = a;
                struct Node* b_copy = b;
                struct Node* c_copy = c;

                if(after < before){
                    a_copy->prev->next = a_copy->next;
                    a_copy->next->prev = a_copy->prev;
                    b_copy->next = a_copy;
                    a_copy->prev = b_copy;
                    a_copy->next = c_copy;
                    c_copy->prev = a_copy;
                    improvement = 1;

                    // 巡回路をテキストファイルとして出力 
                    //write_tourlist_data("tour1.dat", tourHead);

                    // 巡回路長を画面に出力
                    // printf("%lf\n", tour_length_list(p, tourHead));
                } 
            }
            b = b->next;
            c = b->next;
        }
      }
      a = a->next;
    }
    if(improvement == 0){
        return;
    }
  }
}

// or-opt-2近傍法による改善
void or_opt_2(struct point p[MAX_N], int n, struct Node* tourHead, int m, int prec[MAX_N]) {
  //printf("or-pot-2近傍法の実行\n");
  int improvement = 1;
  int label[MAX_N];

  for(int a=0;a<n;a++) label[a]=0;
  for(int a=0;a<m;a++) label[prec[a]]=1;

  while(improvement) {
    improvement = 0;
    struct Node* a1 = tourHead->next->next;
    while(a1->next->next->next != NULL){
      struct Node* a2 = a1->next;
      if(label[a1->index] != 1 && label[a2->index] != 1){
        struct Node* b = tourHead->next;
        struct Node* c = b->next;
        while(c->next != NULL){
            if(b != a1 && c != a1 && b != a2 && c != a2){
                double before = dist(p[a1->prev->index],p[a1->index]) + dist(p[a2->index],p[a2->next->index]) + dist(p[b->index],p[c->index]);
                double after = dist(p[a1->prev->index],p[a2->next->index]) + dist(p[b->index],p[a1->index]) + dist(p[a2->index],p[c->index]);

                struct Node* a1_copy = a1;
                struct Node* a2_copy = a2;
                struct Node* b_copy = b;
                struct Node* c_copy = c;

                if(after < before){
                    a1_copy->prev->next = a2_copy->next;
                    a2_copy->next->prev = a1_copy->prev;
                    b_copy->next = a1_copy;
                    a1_copy->prev = b_copy;
                    a2_copy->next = c_copy;
                    c_copy->prev = a2_copy;
                    improvement = 1;

  

                    // 巡回路をテキストファイルとして出力 
                    //write_tourlist_data("tour.dat", tourHead);

                    // 巡回路長を画面に出力
                    // printf("%lf\n", tour_length_list(p, tourHead));
            
                } 
            }
            b = b->next;
            c = b->next;
        }
      }
        a1 = a1->next;
    }
    if(improvement == 0){
        return;
    }
  }
}

// or-opt-k近傍法による改善
void or_opt_k(struct point p[MAX_N], int n, struct Node* tourHead, int m, int prec[MAX_N], int k) {
  int improvement = 1;
  int label[MAX_N];

  for(int a=0;a<n;a++) label[a]=0;
  for(int a=0;a<m;a++) label[prec[a]]=1;

  for(; k > 1; k--){
    // printf("or-pot-%d近傍法の実行\n", k);
    while(improvement) {
      improvement = 0;
      struct Node* a1 = tourHead->next->next;
      struct Node* hantei1 = a1->next->next;

      while(hantei1->next != NULL){
        int preccount = 0;
        struct Node* a2 = a1;
        for(int i = 0; i < k-1; i++) a2 = a2->next;
        struct Node* hantei2 = a1;
        for(int i = 0; i < k; i++){
          if(preccount == 0){
            preccount += label[hantei2->index];
            hantei2 = hantei2->next;          
          }
        }
        if(preccount < 1){
          struct Node* b = tourHead->next;
          struct Node* c = b->next;
          while(c->next != NULL){
            struct Node* hantei3 = c;
            int cnt_hantei3 = 0;
            for(int i = 0; i < k - 3; i++){
              if(hantei3->next == a2 && cnt_hantei3 == 0){
                cnt_hantei3 += 1;
              }
              if(hantei3->next != NULL){
                hantei3 = hantei3->next;
              }
            }
            if(b != a1 && c != a1 && b != a2 && c != a2 && cnt_hantei3 < 1){
                double before = dist(p[a1->prev->index],p[a1->index]) + dist(p[a2->index],p[a2->next->index]) + dist(p[b->index],p[c->index]);
                double after = dist(p[a1->prev->index],p[a2->next->index]) + dist(p[b->index],p[a1->index]) + dist(p[a2->index],p[c->index]);

                struct Node* a1_copy = a1;
                struct Node* a2_copy = a2;
                struct Node* b_copy = b;
                struct Node* c_copy = c;

                if(after < before){
                    a1_copy->prev->next = a2_copy->next;
                    a2_copy->next->prev = a1_copy->prev;
                    b_copy->next = a1_copy;
                    a1_copy->prev = b_copy;
                    a2_copy->next = c_copy;
                    c_copy->prev = a2_copy;
                    improvement = 1;

  

                    // 巡回路をテキストファイルとして出力 
                    //write_tourlist_data("tour.dat", tourHead);

                    // 巡回路長を画面に出力
                    // printf("%lf\n", tour_length_list(p, tourHead));
            
                } 
            }
            b = b->next;
            c = b->next;
          }
        }
        a1 = a1->next;
        hantei1 = a1;
        for(int i = 0; i < k; i++){
          hantei1 = hantei1->next;
        }
      }
    }
  // write_tourlist_data("tour1.dat", tourHead);
  // printf("%5.1lf\n", tour_length_list(p, tourHead));
  }
}


void TwoOpt (struct point p[MAX_N], int n, int tour[MAX_N], int m, int prec[MAX_N]) {
  int in_prec[MAX_N];
  int a, b, c, d;
  int i, j, k, l, g, h, success, count;
  int improvement = 1;
  double min;

  for (i=0;i<n;i++) in_prec[i]=0;
  for (i=0;i<m;i++) in_prec[prec[i]]=1;

  while(improvement){

    improvement = 0;

    for(i=0;i<=n-3;i++) {
      j = i + 1;

      count = in_prec[tour[j]];

      for(k =i+2; k <= n-1; k++){
        count += in_prec[tour[k]];
        if(count >= 2) break;
        l= (k+1) % n;

        a = tour[i]; b = tour[j]; c = tour[k]; d = tour[l];
        if(dist(p[a],p[b])+dist(p[c],p[d])>dist(p[a],p[c])+dist(p[b],p[d])) {
          g = j; h = k;
          improvement = 1;
          while(g < k){
            SWAP(tour[g],tour[h]);
            g++; h--;
          }
        }
      }
    }
  }
}


void ThreeOpt(struct point p[MAX_N], int n, int tour[MAX_N],int m,int prec[MAX_N]){
  int a,b,c,d,e,f,min_dist,len,prev_i,next_i;
  int i,j,k,l,g,h,g1,h1,g2,h2,j1,k1,cnt=1,kk,l1_copy,l_copy;
  int tour_tmp[MAX_N],prec_i[MAX_N];
  for(int p=0;p<n;p++)tour_tmp[p]=tour[p];


  for(int p=0;p<n;p++)prec_i[p]=-1;
  
  while(cnt!=0){
    //printf("%d ",cnt);
    cnt=0;
    kk=0;
    for(i=0;i<m;i++){
      for(j=0;j<n;j++){
        //printf("%d %d\n",tour[j],prec[i]);
        if(prec[i]==tour[j]){
          prec_i[prec[i]]=j;
          //printf("%d ",j);
          break;
          
        }
      }
    }
    //for(int ll=0;ll<n;ll++)printf("%d ",prec_i[ll]);

    for(kk=1;kk<m-2;kk++){//順序のループ
    for(int ll=0;ll<m;ll++){
      for(j=0;j<n;j++){
        if(prec[ll]==tour[j]){
          prec_i[prec[ll]]=j;
          break;
        }
      }
    }
    

        prev_i=prec_i[prec[kk-1]];
        next_i=prec_i[prec[kk+1]];
        double min = INF;

        if(next_i-prev_i-1<6)continue;
        
        for(j=prev_i+1;j<next_i-5;j++){//1本目のループ
          //printf("%d--------------\n",prec_i[prec[kk]]);
          for(k=j+2;k<next_i-3;k++){//2本目のループ
            for(l=k+2;l<next_i-1;l++){//３本目ループ

                a=tour[j];b=tour[j+1];
                c=tour[k];d=tour[k+1];
                e=tour[l];f=tour[l+1];

                //printf("%d %d %d %d %d %d\n",a,b,c,d,e,f);
                

                min_dist=dist(p[a],p[b])+dist(p[c],p[d])+dist(p[e],p[f]);
                int t=0,improvement=0;
                if(min_dist>dist(p[a],p[b])+dist(p[c],p[e])+dist(p[d],p[f])){
                    t=2;
                    g=k+1;h=l;
                    min_dist=dist(p[a],p[b])+dist(p[c],p[e])+dist(p[d],p[f]);
                }
                if(min_dist>dist(p[a],p[c])+dist(p[b],p[d])+dist(p[e],p[f])){
                    t=2;
                    g=j+1;h=k;
                    min_dist=dist(p[a],p[c])+dist(p[b],p[d])+dist(p[e],p[f]);
                }
                if(min_dist>dist(p[a],p[c])+dist(p[b],p[e])+dist(p[d],p[f])){
                    t=1;
                    g1=j+1;h1=k;
                    g=k+1;h=l;
                    min_dist=dist(p[a],p[c])+dist(p[b],p[e])+dist(p[d],p[f]);
                    
                }
                if(min_dist>dist(p[a],p[d])+dist(p[e],p[c])+dist(p[b],p[f])){
                    t=1;
                    g1=k+1;h1=l;
                    g=j+1;h=l;
                    min_dist=dist(p[a],p[d])+dist(p[e],p[c])+dist(p[b],p[f]);
                }
                if(min_dist>dist(p[a],p[d])+dist(p[e],p[b])+dist(p[c],p[f])){
                    t=3;
                    g2=j+1;h2=k;
                    g1=k+1;h1=l;
                    g=j+1;h=k;
                    min_dist=dist(p[a],p[d])+dist(p[e],p[b])+dist(p[c],p[f]);
                }
                if(min_dist>dist(p[a],p[e])+dist(p[d],p[b])+dist(p[c],p[f])){
                    t=1;
                    g1=k;h1=j+1;
                    g=j+1;h=l;
                    min_dist=dist(p[a],p[e])+dist(p[d],p[b])+dist(p[c],p[f]);
                }
                switch(t){
                    case 0:
                     break;
                    case 3:
                     while(g2<h2){
                        SWAP(tour[g2],tour[h2]);
                        g2++;h2--;
                     }
                    case 1:
                     while(g1<h1){
                        SWAP(tour[g1],tour[h1]);
                        g1++;h1--;
                     }
                    // printf("1----");
                    case 2:
                    // printf("2----\n");
                     while(g<h){
                      SWAP(tour[g],tour[h]);
                      g++;h--;
                     }

                     improvement=1;
                     break;
                }   

                if (improvement == 1) {
                    for(int ll=0;l<n;l++)
                     if(tour[ll]==prec[kk])prec_i[tour[ll]]=ll;
                   
                  
                 // 巡回路をテキストファイルとして出力
                 //write_tourarray_data("tour1.dat", n, tour);
                 // 巡回路長を画面に出力
                 //printf("%lf\n", tour_length_array(p, n, tour));
                }
            }
          }
        }        
        
    }
    
  }
}

void DoubleBridge(struct point p[MAX_N], int n, int tour[MAX_N],int m,int prec[MAX_N],int s){
  int now_j,prev_j,stl,min_length=INF,next_j,step,g,h,random_m;
  int prec_i[MAX_N],kk=0;
  for(int i=0;i<m;i++){
    while(prec[i] != tour[kk])kk++;
    prec_i[prec[i]]=kk;
    //printf("%d %d---",prec_i[prec[i]],prec[i]);
  }
  do{
    random_m = rand()%(m+1);
  }while(random_m+s>=m || random_m < 2);
  kk=0;
  now_j=prec_i[prec[2]];

  for(int i=random_m;i<random_m+s;i++){
    
    prev_j=prec_i[prec[i-1]];
    if(i==m-1)next_j=n;
    else next_j=prec_i[prec[i+1]];
    //printf("%d %d---------------------\n",j,i);
    if(next_j-prev_j-2<11);
    else{
        //printf("%d %d---\n",prev_j+1,next_j-1);
        
      stl=next_j-prev_j-2;
      step=stl/8;

      //for(int l=prev_j;l<next_j+1;l++)printf("%d ",tour[l]);printf("\n");

      g=2*step+prev_j;h=3*step+prev_j;
      //printf("%d %d ",g,h);
      while(g<h){
        SWAP(tour[g],tour[h]);
        g++;h--;
      }
      g=4*step+prev_j;h=5*step+prev_j;
      //printf("%d %d ",g,h);
      while(g<h){
        SWAP(tour[g],tour[h]);
        g++;h--;
      }
      g=6*step+prev_j;h=7*step+prev_j;
      //printf("%d %d \n",g,h);
      while(g<h){
        SWAP(tour[g],tour[h]);
        g++;h--;
      }
      g=2*step+prev_j;h=7*step+prev_j;
      while(g<h){
        SWAP(tour[g],tour[h]);
        g++;h--;
      }

      while(prec[i]!=tour[kk])kk++;
      prec_i[prec[i]]=kk;
      now_j=kk;
      
      //for(int l=prev_j;l<next_j+1;l++)printf("%d ",tour[l]);printf("\n");
        
    }
  }
}

void AllDoubleBridge(struct point p[MAX_N], int n, int tour[MAX_N],int m,int prec[MAX_N],int s){
  int now_j,prev_j,stl,min_length=INF,next_j,step,g,h,random_m;
  int prec_i[MAX_N],kk=0;
  for(int i=0;i<m;i++){
    while(prec[i] != tour[kk])kk++;
    prec_i[prec[i]]=kk;
    //printf("%d %d---",prec_i[prec[i]],prec[i]);
  }
  kk=0;
  now_j=prec_i[prec[2]];

  for(int i=s;i<10+s;i++){
    
    prev_j=prec_i[prec[i-1]];
    if(i==m-1)next_j=n;
    else next_j=prec_i[prec[i+1]];
    //printf("%d %d---------------------\n",j,i);
    if(next_j-prev_j-2<11);
    else{
        //printf("%d %d---\n",prev_j+1,next_j-1);
        
      stl=next_j-prev_j-2;
      step=stl/8;

      //for(int l=prev_j;l<next_j+1;l++)printf("%d ",tour[l]);printf("\n");

      g=2*step+prev_j;h=3*step+prev_j;
      //printf("%d %d ",g,h);
      while(g<h){
        SWAP(tour[g],tour[h]);
        g++;h--;
      }
      g=4*step+prev_j;h=5*step+prev_j;
      //printf("%d %d ",g,h);
      while(g<h){
        SWAP(tour[g],tour[h]);
        g++;h--;
      }
      g=6*step+prev_j;h=7*step+prev_j;
      //printf("%d %d \n",g,h);
      while(g<h){
        SWAP(tour[g],tour[h]);
        g++;h--;
      }
      g=2*step+prev_j;h=7*step+prev_j;
      while(g<h){
        SWAP(tour[g],tour[h]);
        g++;h--;
      }

      while(prec[i]!=tour[kk])kk++;
      prec_i[prec[i]]=kk;
      now_j=kk;
      
      //for(int l=prev_j;l<next_j+1;l++)printf("%d ",tour[l]);printf("\n");
        
    }
  }
}


// 挿入近傍法による改善
void Addin(struct point p[MAX_N], int n, int tour[MAX_N],int m,int prec[MAX_N]) {
 
  int label[MAX_N],log,min_length,before,after,g,h,cnt=0;
  

  

  for(int a=0;a<n;a++) label[a]=0;
  for(int a=0;a<m;a++) label[prec[a]]=1;

  for(int i=1;i<n-1;i++){
    if(label[tour[i]]==1)continue;
    cnt=0;
    min_length=0;

    for(int j=0;j<n-1;j++){
        
        if(i==j)continue;
        before=dist(p[tour[i]],p[tour[i-1]])+dist(p[tour[i]],p[tour[i+1]])+dist(p[tour[j]],p[tour[j+1]]);
        after=dist(p[tour[i]],p[tour[j]])+dist(p[tour[i]],p[tour[j+1]])+dist(p[tour[i-1]],p[tour[i+1]]);
        log=before-after;
        if(after>before && min_length<log){
            min_length=log;
            g=i;h=j;
            cnt++;
            //printf("af:%d be:%d\n",g,h);
        }
    }
    //printf("be:%f  af:",tour_length_array(p,n,tour));
    if(cnt>0&&g<h){
        //printf("%d %d---------------------\n",tour[g],tour[h]);
        while(g<h){
            SWAP(tour[g],tour[g+1]);
            //printf("%d %d\n",tour[g],tour[g+1]);
            g++;
        }
    }else if(cnt>0&&g>h){
        while(g>h){
            SWAP(tour[g-1],tour[g]);
            g--;
        }
    }
    //printf("%f\n",tour_length_array(p,n,tour));
  }
  
}




int main(int argc, char *argv[]) {
  int n, min=INF, nice=0, attack, prev_min=0;
  int m;
  struct point p[MAX_N];
  int tour[MAX_N],besttour[MAX_N],RandBest[MAX_N];
  struct Node* tourHead;
  int prec[MAX_N];
  int i,I=0,bad=0,bad_cnt=1;
  int strong;
  int omg=0,K=15;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <tsp_filename>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  read_tsp_data(argv[1], p, &n, prec, &m);
  if(n<1000){
    strong=3;
    bad_cnt=3;
  }else if(n<1300)strong=4;
  else {
    strong=6;
    K=10;
  }
 int kai=100-strong*10;
  // ciによる巡回路構築
  ci(p,n,tour,m,prec);
  write_tourarray_data("tour1.dat", n, tour);
  min=tour_length_array(p,n,tour);
  printf("%5.1lf\n", tour_length_array(p, n, tour));

  
  while(1){
    nice=0;
    int good=0;

    //3-optによる改善
    
    ThreeOpt(p, n, tour, m, prec);
    if(min>tour_length_array(p,n,tour)){
        printf("3optによる改善\n");
        write_tourarray_data("tour1.dat", n, tour);
        printf("%5.1lf\n", tour_length_array(p, n, tour));
        min=tour_length_array(p,n,tour);
        nice++;
        for(int i=0;i<n;i++)besttour[i]=tour[i];
    }else if(prev_min-1>tour_length_array(p,n,tour))good++;

    // 2optによる改善
    TwoOpt(p, n, tour, m, prec);
    if(min>tour_length_array(p,n,tour)){
        printf("2optによる改善\n");
        write_tourarray_data("tour1.dat", n, tour);
        printf("%5.1lf\n", tour_length_array(p, n, tour));
        min=tour_length_array(p,n,tour);
        nice++;
        for(int i=0;i<n;i++)besttour[i]=tour[i];
    }else if(prev_min-1>tour_length_array(p,n,tour))good++;


    tourHead = arrayToList(tour, n);

    // or-opt-k以下の近傍法による改善
    for(int k=K;1<k;k--){
     //printf("or-pot-%d以下近傍法の実行\n", k);
    or_opt_k(p, n, tourHead, m, prec, k);
    if(min>tour_length_list(p,tourHead)){
        write_tourlist_data("tour1.dat", tourHead);
        printf("or-pot-%d以下近傍法の実行\n", k);
        printf("%5.1lf\n", tour_length_list(p, tourHead));
        min=tour_length_list(p, tourHead);
        for(int i=0;i<n;i++)besttour[i]=tour[i];
    }
    }
    

    // or-opt-1近傍法による改善
  
    or_opt_1(p, n, tourHead, m, prec);
    listToArray(tourHead,tour);
    freeTour(tourHead); // メモリの解放
    if(min>tour_length_array(p,n,tour)){
        printf("or-opt-1による近傍法の実行\n");
        write_tourarray_data("tour1.dat", n, tour);
        printf("%5.1lf\n", tour_length_array(p, n, tour));
        min=tour_length_array(p,n,tour);
        nice++;
        for(int i=0;i<n;i++)besttour[i]=tour[i];
    }else if(prev_min-1>tour_length_array(p,n,tour))good++;

    // printf("%d : %f :",j+1,tour_length_list(p,tourHead));
     prev_min=tour_length_array(p,n,tour);

    I++;
    //printf("%d : %f --omg:%d--",I,tour_length_array(p,n,tour),omg);

    if(I%24 == 0  && K>5)K--;


    if(nice>0){
        attack=1;
        bad=0;
        omg=0;
    }else if(good>0){
      bad++;
    }else if(omg==6){
        int rand_min=0;
        //printf("rand-bunbun");
        if(kai<1)kai=30;
        for(int l=0;l<kai;l++){
            re_rnd_ci(p,n,tour,m,prec);

            if(rand_min<tour_length_array(p,n,tour)){
                //printf("or-opt-1による近傍法の実行\n");
                //printf("%5.1lf\n", tour_length_array(p, n, tour));
                rand_min=tour_length_array(p,n,tour);
                for(int i=0;i<n;i++)RandBest[i]=tour[i];
                for(int i=0;i<n;i++)tour[i]=besttour[i];
            }
            K++;
        }
        for(int l=0;l<n;l++)tour[l]=RandBest[l];
        omg=0;kai-=5;
    }else if(bad>bad_cnt&& omg != 4&& omg !=0 && omg != 2){
        //printf("--strong kick--");
        
        DoubleBridge(p,n,tour,m,prec,50);
        
        DoubleBridge(p,n,tour,m,prec,50);
        bad=0;
        omg++;
    }else if(omg==4 && bad>bad_cnt){
        for(int l=0;l<n;l++)tour[l]=besttour[l];
        re_ci(p,n,tour,m,prec);
        //printf("--best--re_ci-----");
        bad=0;
        attack=1;
        omg++;
        
    }
    else if(omg > 1 && bad>bad_cnt){
      //for(int l=0;l<n;l++)tour[l]=besttour[l];
      re_rnd_1_ci(p,n,tour,m,prec);
      //printf("--re_rnd_1_ci---");
      bad=0;attack=1;
      omg++;
    }else if(bad>bad_cnt){
      
      re_ci(p,n,tour,m,prec);
      //printf("--re_ci-----");
      bad=0;
      attack=1;
      omg++;
      
    }else {
        attack+=strong;
        bad++;
        if(attack ==4*strong){
            for(int i=0;i<n;i++)tour[i]=besttour[i];
            attack=3*strong;
            if(n<1500&&n>1000){
              for(int i=0;i<2;i++)
               DoubleBridge(p, n, tour, m, prec,attack);
            }
            DoubleBridge(p, n, tour, m, prec,attack);  
        }else if(attack>=3*strong){
          attack=3*strong;
          

        }
    }

    // DoubleBridgeによるキック
    //printf("DoubleBridgeによるキック\n");
    DoubleBridge(p, n, tour, m, prec,attack);
    //printf("---attack--%d--\n",attack);


    //write_tourarray_data("tour.dat", n, tour);
    //printf("%5.1lf\n", tour_length_array(p, n, tour));
   
  }
  // write_tourarray_data("tour1.dat", n, tour);
  // printf("%d\n",min);

  
  exit(EXIT_SUCCESS);
}
