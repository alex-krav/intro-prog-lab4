# Assignment #4. Binary files. OOP. Encapsulation

## Варіант #1. Обробка зображень
Завдання полягає в збільшенні зображення в задану цілу кількість разів. На вхід програмі подаються імена вхідного та вихідного файлів, а також число. Ваша програма має працювати з форматом BMP. Для збільшення зображення не обов\'язково використовувати складні алгоритми – достатньо лише кілька разів копіювати пікселі.

*Складніше завдання (+1 бал):* Збільшувати та зменшувати зображення в нецілу кількість разів, використовуючи інтерполяцію

### Формат файлів BMP
Програма має працювати з bmp файлами, які зберігають колір у форматі 24 бітів на піксель. 

*Типи intX_t використовуються для представлення цілих чисел з незалежним від розрядності ОС розміром. На свій страх і ризик ви можете використовувати типи відповідного розміру char, int, short, long, byte, які насправді залежать від розрядності вашої ОС та опцій компіляції.*

Файл починається із заголовка, який можна представити у вигляді C-структури
```C
typedef struct {
     int8_t id[2];            // Завжди дві літери 'B' і 'M'
->   int32_t filesize;        // Розмір файла в байтах
     int16_t reserved[2];     // 0, 0
     int32_t headersize;      // 54L для 24-бітних зображень
     int32_t infoSize;        // 40L для 24-бітних зображень
->   int32_t width;           // ширина зображення в пікселях
->   int32_t depth;           // висота зображення в пікселях
     int16_t biPlanes;        // 1 (для 24-бітних зображень)
     int16_t bits;            // 24 (для 24-бітних зображень)
     int32_t biCompression;   // 0L
     int32_t biSizeImage;     // Можна поставити в 0L для зображень без компрессії (наш варіант)
     int32_t biXPelsPerMeter; // Рекомендована кількість пікселів на метр, можна 0L
     int32_t biYPelsPerMeter; // Те саме, по висоті
     int32_t biClrUsed;       // Для індексованих зображень, можна поставити 0L
     int32_t biClrImportant;  // Те саме
     } BMPHEAD;
```
Скоріше за все вам знадобляться лише поля, позначені стрілкою. Далі йде інформація про пікселі у вигляді масиву з таких структур:
```C
typedef struct {
     int8_t redComponent;
     int8_t greenComponent;
     int8_t blueComponent;
} PIXELDATA;
```
Через неймовірну винахідливість авторів формату розмір у байтах кожного рядка пікселів має ділитися на 4. Тому якщо кількість пікселів у рядку помножена на 3 (розмір PIXELDATA) не ділиться на 4, необхідно дописувати ще кілька нульових байтів у кінець кожного рядка. Докладніше про те, як це зробити, можете прочитати [тут](https://www.siggraph.org/education/materials/HyperVis/asp_data/compimag/bmpfile.htm)

### Вхідні та вихідні дані
Припустимо, що ваша програма компілюється в resize.exe. Тоді її треба запускати наступним чином:
```
> resize.exe input.bmp output.bmp 3
Enlarging image 3 times... Done.
Written result to output.bmp
```
Приклад [вхідного](./examples/bmp.bmp) та [вихідного](./examples/bmp5x.bmp) зображень, а також дамп початкових байтів першого зображення у hex-редакторі *(заголовки позначені виділенням)*: ![](./examples/bmp_hex.PNG)

### Посилання
* [RGB](https://en.wikipedia.org/wiki/RGB_color_model#Numeric_representations)-кодування
* [Формат BMP](https://en.wikipedia.org/wiki/BMP_file_format) на вікі
* [Докладний](http://www.digicamsoft.com/bmp/bmp.html) опис формату
* [Ще один](http://www.dragonwins.com/domains/getteched/bmp/bmpfileformat.htm) простий опис
* [Схоже завдання](http://docs.cs50.net/problems/whodunit/whodunit.html) на cs50, із ну дуже простим поясненням формату
* [Білінійна інтерполяція](https://en.wikipedia.org/wiki/Bilinear_interpolation), в статті також йдеться про інші види інтерполяцій. Ви можете використати будь-яку з них, проте для зображень підходять лише 2-вимірні, наприклад бікубічна. У програмах обробки зображень, наприклад, Photoshop, використовується також [фільтр Ланцоша](https://en.wikipedia.org/wiki/Lanczos_resampling).
