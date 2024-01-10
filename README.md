USART DRIVER\
in this project I'm using STM32F4xx\
Interrupt is generated in case of parity error or receiver buffer not empty which indicate that the buffer is ready to be read\
the process  starts on reset the micro will send "hello world" then wait for command {"turn_(on/off)" , "(green/red)_led" } \
when command is received the micro will act as a slave, it will respond to the command then send "Done" as acknowledge .
