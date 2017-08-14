#include <stdio.h>

#include <sapi/sys.hpp>
#include <sapi/hal.hpp>
#include <sapi/var.hpp>


static void print_all();
static void show_usage();

int main(int argc, char * argv[]){
	String operation;

	Cli cli(argc, argv);
	cli.set_publisher("Stratify Labs, Inc");
	cli.handle_version();


	if( cli.is_option("--help") || cli.is_option("-h") ){
		show_usage();
		exit(0);
	}

	operation = cli.at(1).c_str();

	if( operation == "readall" ){
		printf("%s readall\n\n", cli.name());
		print_all();
	} else if ( operation == "read" ){

		mcu_pin_t pio = cli.pin_at(2);

		if( pio.port != 255 ){
			Pin p(pio.port, pio.pin);
			if( p.open(Pin::RDWR) < 0 ){
				printf("Failed to open /dev/pio%d\n", pio.port);
			} else {
				printf("%d.%d == %d\n", pio.port, pio.pin, p.get_value());
				p.close();
			}
		} else {
			show_usage();
		}
	} else if ( operation == "write" ){
		mcu_pin_t pio = cli.pin_at(2);
		int value = cli.value_at(3);

		if( pio.port != 255 ){
			Pin p(pio.port, pio.pin);
			if( p.open(Pin::RDWR) < 0 ){
				printf("Failed to open /dev/pio%d\n", pio.port);
			} else {
				if( value == 0 ){
					p.clear();
				} else {
					value = 1;
					p.set();
				}
				printf("%d.%d -> %d\n", pio.port, pio.pin, value);
				p.close();
			}
		} else {
			show_usage();
		}

	} else if ( operation == "mode" ){

		if( cli.size() != 4 ){
			show_usage();
		} else {

			mcu_pin_t pio = cli.pin_at(2);
			String mode = cli.at(3);

			if( pio.port == 255 ){
				show_usage();
			} else {
				Pin p(pio.port, pio.pin);
				if( p.open(Pin::READWRITE) < 0 ){
					printf("Failed to open /dev/pio%d", pio.port);
				} else {


					if( (mode == "in") || (mode == "float") || (mode == "tri") ){
						p.set_attr(Pin::FLAG_SET_INPUT | Pin::FLAG_IS_FLOAT);
						printf("%d.%d -> in\n", pio.port, pio.pin);
					} else if ( mode == "out" ){
						p.set_attr(Pin::FLAG_SET_OUTPUT);
						printf("%d.%d -> out\n", pio.port, pio.pin);
					} else if ( (mode == "up") || (mode == "pullup") ){
						p.set_attr(Pin::FLAG_SET_INPUT | Pin::FLAG_IS_PULLUP);
						printf("%d.%d -> pullup\n", pio.port, pio.pin);
					} else if ( (mode == "down") || (mode == "pulldown") ){
						p.set_attr(Pin::FLAG_SET_INPUT | Pin::FLAG_IS_PULLDOWN);
						printf("%d.%d -> pulldown\n", pio.port, pio.pin);
					}
				}
			}
		}
	} else if ( operation == "pulse" ){
		if( (cli.size() != 5) ){
			show_usage();
		} else {
			mcu_pin_t pio = cli.pin_at(2);
			int value = cli.value_at(3);
			int t = cli.value_at(4);

			if( value != 0 ){
				value = 1;
			}

			if( pio.port == 0xff ){
				show_usage();
			} else {
				Pin p(pio.port, pio.pin);
				if( p.open(Pin::READWRITE) < 0 ){
					printf("Failed to open /dev/pio%d", pio.port);
				} else {
					//go high then low
					printf("%d.%d -> %d (%dusec) -> %d\n", pio.port, pio.pin, value, t, !value);
					p = (value != 0);
					Timer::wait_usec(t);
					p = (value == 0);
				}
			}
		}
	}

	return 0;
}


void print_all(){
	int port_num = 0;
	int pin;
	u32 value;
	//show the status of all the pins
	printf("     28   24   20   16   12    8    4    0\n");
	printf("  ----------------------------------------\n");
	do {
		Pio p(port_num);

		if( (p.open() < 0) || (port_num > 10) ){
			break;
		}

		printf("P%d|", port_num);
		for(pin = 0; pin < 32; pin++){
			value = p.get_value();
			if( value & (1<<(31-pin)) ){
				printf("1");
			} else {
				printf("0");
			}

			if( (pin+1) % 4 == 0 ){
				printf(" ");
			}
		}
		printf("\n");

		port_num++;

	} while(1);

	printf("\n");

}

void show_usage(){
	printf("Usage:\n");
	printf("\tgpio readall|mode|read|write\n");
	printf("\tgpio readall\n");
	printf("\tgpio mode X.Y in|out|up|down\n");
	printf("\tgpio write X.Y 1|0\n");
	printf("\tgpio read X.Y\n");
	printf("\tgpio pulse X.Y 1|0 usec\n");
	exit(0);
}
