#include <stdio.h>

#include <stfy/sys.hpp>
#include <stfy/hal.hpp>
#include <stfy/var.hpp>


static void print_all();
static void show_usage();


int main(int argc, char * argv[]){
	String operation;

	Cli cli(argc, argv);
	cli.set_publisher("Stratify Labs, Inc");
	cli.set_version("1.0");

	if( cli.is_option("--version") || cli.is_option("-v") ){
		cli.print_version();
		exit(0);
	}

	if( cli.is_option("--help") ){
		show_usage();
		exit(0);
	}

	operation = cli.at(1).c_str();

	if( operation == "readall" ){
		print_all();
	} else if ( operation == "read" ){

		pio_t pio = cli.pio_at(2);

		if( pio.port != 255 ){
			Pin p(pio.port, pio.pin);
			if( p.open(Pin::RDWR) < 0 ){
				printf("Failed to open /dev/pio%d\n", pio.port);
			} else {
				printf("%d.%d == %d\n", pio.port, pio.pin, p.value());
				p.close();
			}
		} else {
			show_usage();
		}
	} else if ( operation == "write" ){
		pio_t pio = cli.pio_at(2);
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

			pio_t pio = cli.pio_at(2);
			String mode = cli.at(3);

			if( pio.port == 255 ){
				show_usage();
			} else {
				Pin p(pio.port, pio.pin);
				if( p.open(Pin::READWRITE) < 0 ){
					printf("Failed to open /dev/pio%d", pio.port);
				} else {


					if( (mode == "in") || (mode == "float") || (mode == "tri") ){
						p.set_attr(Pin::INPUT | Pin::FLOAT);
						printf("%d.%d -> in\n", pio.port, pio.pin);
					} else if ( mode == "out" ){
						p.set_attr(Pin::OUTPUT);
						printf("%d.%d -> out\n", pio.port, pio.pin);
					} else if ( (mode == "up") || (mode == "pullup") ){
						p.set_attr(Pin::INPUT | Pin::PULLUP);
						printf("%d.%d -> pullup\n", pio.port, pio.pin);
					} else if ( (mode == "down") || (mode == "pulldown") ){
						p.set_attr(Pin::INPUT | Pin::PULLDOWN);
						printf("%d.%d -> pulldown\n", pio.port, pio.pin);
					}
				}
			}
		}
	}


	return 0;
}


void print_all(){
	//show the status of all the pins
	printf("SHOW ALL PINS\n");


}

void show_usage(){
	printf("Usage:\n");
	exit(0);
}
