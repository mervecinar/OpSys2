#include <pthread.h>//thread için
#include  <string.h>
#include  <stdio.h>
#include  <stdlib.h>
#include <unistd.h>//sleep
#include <time.h>//random rakam için
#include <semaphore.h>//semafor için


double princmiktar=2000;// bize verilen prinç miktarı

int i,m,j, h=0;
struct Masa//bir tane struct yapısı ile masa oluşturuldu
{
	sem_t semf;// bekleme ve devam etme için semafor tutuluyor.
};
struct filozof_struct//filozof için struct yapısı
{
	int x;
	int sayac_yeme;//kaç kere yediği
	int sayac_dusunme;// kaç kere düşündüğü
	int m;
	pthread_t id;// id tutuluyor
	int y;	
};

struct Masa* ms;// structtan pointer üretiliyor
sem_t blok;

int kontrol()
{
// bu yapı dur ve devam et blok yapısıdır bunun içinde sem wait ile eğer aktif olan thread var ise beklemektedir.
	int l = 0;
	sem_wait(&blok);
	
	l = h;
	// bu yapı ise devam et yapısı post ile eğer bir önceki threadin işi bittiyse devam et demektir blok burada açılır
	sem_post(&blok);	
	return l==0;
	// buna semafor yapısı denmektedir.
}

// aşşağıda fonksiyon imzalarım bulunmakta
int kontrol();
void filozofdusunme(struct filozof_struct* flzf);
void* anafonk(void *s);
int kullanicinput();
void  filozof_atama (int count, struct filozof_struct* flzf);
void semafor(int count, struct filozof_struct* flzf);


int main(void){

	int sayi= kullanicinput();// kullanıcıdan grup sayısı için input alınıyor
	printf("sayi: %d", sayi);
	
	struct filozof_struct* flzf;
	int count = sayi*8;// burası total üretilecek olan filozof sayısı
	flzf = (struct filozof_struct*) malloc(sizeof(struct filozof_struct) * count);// filozoflar için malloc yapısı ile bellekten yer ayırtılmakta.
        sem_init(&blok,0,1);
	ms = (struct Masa*)malloc(sizeof(struct Masa) * count);//burada da masalar için yer ayırtılmakta
	h = count;
        semafor(count,flzf );
	filozof_atama(count,flzf );

    printf("-------------------------SONUC---------------------------");
	
	int c=0;
	int n=0;
	double masaucret=99.90;// masa direk 99.90 tl açıldığı için bu değer.
	
	m=0;
	for(i=0;i<count;i++){ //filozof sayısı kadar gidiyor
		if(c==8){// her masaya 8 kişi oturacağı için 8 de bir masaları yazdırıyor durumunu.
    		  	printf("-------------MASA [%d] ---------------\n",n);
    		  	printf("Masa tazeleme  %d\n",m);
    			printf("Masa Ucreti: %.1f\n", masaucret);
    			//8 de bir masaların durumu 0 lanması için eski değerler atılıyor.
    			n++;
    			c=0;
    			m=0;
    			masaucret=99.90;
    			princmiktar=2000;
    			
    		  	}
  
		if(flzf[i].sayac_yeme*100<princmiktar){//Buurada tencerede kalan prinç yeterli mi kontrolü sağlanıyor eğer yeterli ise sorun yok filozzof yiyebilir fakat yetersizise
		printf("Filozof ID:   [  %d  ] ||    Dusunme:   %d  ||   Yeme:  %d gram \n", flzf[i].m, flzf[i].sayac_dusunme, flzf[i].sayac_yeme*100);

			princmiktar=princmiktar-(flzf[i].sayac_yeme*100);
			c++;
			masaucret= masaucret+=(flzf[i].sayac_yeme)*2; //1000 gr 20 tl ise 100 gr princ 2 tl 
		       
		}else{// masa tazelenmeli tazelenme sayısı da artmalı.20000 g pirinç veriliyor tekrar.
			   m++;
		        princmiktar=2000;
		
		}

    			if(m!=0){// eğer tazelenme olduysa
    			masaucret=masaucret+(m*(19.90));// masa fiyatına 20 tl eklenir
    			}
    			
    		  
    }
    	
}

  void filozofdusunme(struct filozof_struct* flzf){// filozof burada düşünmektedir bu süreçte 1 ila 5 saniye arası beklemektedir.
         flzf->sayac_dusunme++;
          printf("Filozof ID:   [ %d  ] ||   Kac Kez Dusundu:   %d  ||   Kac Kez Yedi:  %d \n", flzf -> m, flzf -> sayac_dusunme , flzf -> sayac_yeme);
         sleep(rand() % 5 + 1 );
         flzf->sayac_dusunme++;// düşünme sayacı artar.
}


void* anafonk(void *s){	
int o=0;
	struct filozof_struct* flzf = (struct filozof_struct*)s;//pointer üretiliyor filozof türünde.
	while(!kontrol())// kontrol dediğim yemeyen kaldı mı yemeyen kaldığında bir oluyor ve içerisi devam ediyor.
{ 
          filozofdusunme(flzf);// yemek yemediğinde düşünüyor;
		if (sem_wait(&ms[flzf->x].semf)==0)
		{// eğer kaşık ilgilide değilse beklemesi gerekiyor.
			int sure =rand() % 5 +1 ; 
                       while(sure>0){
                        if (sem_wait(&ms[flzf->y].semf)==0)
				{	
						//burada ne kadar düşünmüş ve bu zamana kadar kaç kere yemiş print ediyorum kullanıcı ne işlem olduğunu görür.		
					 printf("Filozof ID:   [ %d  ] ||   Kac Kez Dusundu:   %d  ||   Kac Kez Yedi:  %d \n", flzf -> m, flzf -> sayac_dusunme , flzf -> sayac_yeme);
					
					if (!flzf->sayac_yeme){
					// bu bir semafor yapısıdır bu yapı sayesinde thread lar bir bir yer ve kaşığı işi bitince post ile diğerine bırakır

						sem_wait(&blok);
						
						h--;
						
						sem_post(&blok);
					}

					flzf->sayac_yeme++;// post olduğundanişlem yapılabiliyor

					sem_post(&ms[flzf->y].semf);

					sure =- sure;
				} 
 
			sem_post(&ms[flzf->x].semf);// diğer filozofa bırakma kaşığı
		}
		
		
		
	
		 }}}
		 
int kullanicinput(){// grup saysısı input alınır.
int a;
printf("Bir sayi giriniz: ");
scanf("%d", &a);
return a;
}
void  filozof_atama (int count, struct filozof_struct* flzf){
int i;
// burada filozoflar üretilir 8 * grupnumarası kadar ve tek tel pointer içi doldurulur 
 	for(i=0;i<count;i++)
        pthread_create(&flzf[i].id, NULL, anafonk, &flzf[i]);

	for(i=0;i<count;i++)
	     pthread_join(flzf[i].id, NULL);


}
void semafor(int count, struct filozof_struct* flzf){

	for(i=0; i<count; i++)// burada grup sayısı * 8 kaddar tüm thread lere gezer.
	{
	        sem_init(&ms[i].semf,0,1);  
		flzf[i].sayac_yeme = 0;
		flzf[i].sayac_dusunme = 0;// burada da yine semafor mutex yapısı ile yeme düşünme olayı kontrol altına alınır ve
// bir sonraki thread de 0 lanir çünkü yeni thread eski veriler ile devaö etmesin diye
		flzf[i].m = i + 1;
		flzf[i].x = i;

		if (i+1==count){
			flzf[i].y = 0;
	}else{
			flzf[i].y = i+1;}
	}
}


