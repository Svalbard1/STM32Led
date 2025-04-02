# STM32Led

Прект по управлению светодиодами через виртуальный COM порт: Включение, выключение, мигание.
--------------------------------------------------------------------------------------------

Плата - STM32F3 Discovery

Настройка PUTTY:

![image](https://github.com/user-attachments/assets/0527cf52-be85-4138-83eb-16d3c9812839)

![image](https://github.com/user-attachments/assets/fa047f3f-5420-45c3-921d-3c5a1dc73f80)

![image](https://github.com/user-attachments/assets/bcf2f288-7919-40dd-b9a6-ad9a4695e92c)

Комманды: 

          E(Номер светодиода от 3 до 10) - включение светодиода

          D(Номер светодиода от 3 до 10) - выключение светодиода
          
          B(Номер светодиода),(Частота мигания) - включение мигающего светодиода

Пример комманд: 

          E4
          D4
          B4,100

Пример использования:

<video src='https://1024terabox.com/s/11a-lcMxTFd21W8mfyAjHXQ' width=180/>



