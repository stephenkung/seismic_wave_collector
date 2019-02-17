#define sector0 0x00000000



void write_enable();       

void write_disable();       

unsigned char read_status();   

void write_status(unsigned char data);

unsigned char  read_manu_id () ;

unsigned char read_device_id();

void page_write (int addr);

void read_data_func (int addr,int num )  ;

void chip_erase ();

