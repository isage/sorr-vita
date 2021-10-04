//Cleaned up 3/26 CMH

#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================
					INCLUDE FILES
==================================================================================================*/
#include "te9_tf9_hybrid_driver.h"
#include "i2c-dev.h"
/*==================================================================================================
					LOCAL VARIABLES
==================================================================================================*/
#ifdef KX_PLATFORM_LINUX
static int fd = 0;	//local file descriptor for the /dev/i2c interface
static int block = 0;	//flag for block read functionality (1=block, 0=no block)
#endif
static int device = 0;	//flag to indicate device KXTE9=0, KXTF9=1
#ifdef KX_PLATFORM_REX
	LOCAL rex_crit_sect_type tf9_crit_sect;
#else
	int tf9_crit_sect;
#endif
unsigned char gpio_int_type;
/*==================================================================================================
					COMMON FUNCTIONS
==================================================================================================*/
// TODO:
#ifdef KX_PLATFORM_REX
	extern sky_accel_mode_type                  sky_accel_current_mode;
	extern i2c_status_type accel_i2c_write(unsigned char dev_addr, unsigned char addr, unsigned char *data, unsigned char length);
	extern i2c_status_type accel_i2c_read(unsigned char dev_addr, unsigned char addr, unsigned char *data, unsigned char length);
	extern void accel_process_landscape(unsigned char tilt_pos_pre, unsigned char tilt_pos_cur);
	extern void accel_process_updown(unsigned char tilt_pos_pre, unsigned char tilt_pos_cur);
	extern void accel_process_tap(unsigned char tap_mode, unsigned char tap_direction);
 
#elif defined(KX_PLATFORM_FIRMWARE)
	void Wr24C080(U32 slvAddr,U32 addr,U8 data);
	void Rd24C080(U32 slvAddr,U32 addr,U8 *data);

	void _Wr24C080(U32 slvAddr,U32 addr,U8 data);
	void _Rd24C080(U32 slvAddr,U32 addr,U8 *data);
#endif


/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_bytes
DESCRIPTION:    	This function reads data from the Kionix accelerometer in bytes.
ARGUMENTS PASSED:	register address, data pointer, length in number of bytes
RETURN VALUE:		0 = pass; 1 = fail
PRE-CONDITIONS:		KIONIX_ACCEL_init() has been called
POST-CONDITIONS:	None
IMPORTANT NOTES:	Using the i2c_smbus functions requires the inclusion of the i2c-dev.h file 
			available in the lm-sensors package.
==================================================================================================*/
int KIONIX_ACCEL_read_bytes(int reg, char* data, int length)
{
	int status = 0;
	int ret = 0;
	int i;
	KIONIX_EnterCriticalSection(&tf9_crit_sect);
#ifdef KX_PLATFORM_LINUX
	if (block == 1){
		status = i2c_smbus_read_i2c_block_data(fd, reg, length, (__u8*)data);
		if(status < 0){		//status will be number of bytes read, negative if read failed
			//printf("Read failed on register 0x%02x\n", reg);
			KIONIX_LeaveCriticalSection(&tf9_crit_sect);
			return 1;
		}
	}
	else if (block == 0){
		for (i = 0; i < length; i++){
			ret = i2c_smbus_read_byte_data(fd, reg + i);
			if (ret < 0){	//status will be the data retrieved in the byte read
				//printf("Read failed on register 0x%02x\n", (reg + i));
				return 1;
			}
			else data[i] = (char)ret;
		}
	}
	else {
		KIONIX_LeaveCriticalSection(&tf9_crit_sect);
		return 1;
	}
#elif defined(KX_PLATFORM_WIN32) || defined(KX_PLATFORM_WINCE)

#elif defined(KX_PLATFORM_REX)
		status = accel_i2c_read(KIONIX_ACCEL_I2C_SLV_ADDR, reg, &data[0], length);

#elif defined(KX_PLATFORM_FIRMWARE)
	#ifdef USE_I2C_GPIO
	{
		unsigned char wb[2], rb[2];
		wb[0] = reg;
		i2c_read(KIONIX_ACCEL_I2C_SLV_ADDR, &wb[0], 1, &data[0], length);
	}
	#elif defined(USE_I2C_INT)
		for(i=0; i<length; i++)
			Rd24C080((U32)KIONIX_ACCEL_I2C_SLV_ADDR, (U32)reg+i, &data[i]);
	#elif defined(USE_I2C_POLL)
		for(i=0; i<length; i++)
			_Rd24C080((U32)KIONIX_ACCEL_I2C_SLV_ADDR, (U32)reg+i, &data[i]);
	#endif
#endif
	KIONIX_LeaveCriticalSection(&tf9_crit_sect);
	return 0;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_write_byte
DESCRIPTION:		This function writes a byte of data to the Kionix accelerometer.
ARGUMENTS PASSED:	register address, data variable
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	None
IMPORTANT NOTES:   	Using the i2c_smbus_write_byte_data function requires the inclusion of
			the i2c-dev.h file available in the lm-sensors package.
==================================================================================================*/
int KIONIX_ACCEL_write_byte(int reg, int data)
{
	int res=0;
	KIONIX_EnterCriticalSection(&tf9_crit_sect);

#ifdef KX_PLATFORM_LINUX
	res = i2c_smbus_write_byte_data(fd, reg, data);

#elif defined(KX_PLATFORM_WIN32) || defined(KX_PLATFORM_WINCE)

#elif defined(KX_PLATFORM_REX)
	res = accel_i2c_write(KIONIX_ACCEL_I2C_SLV_ADDR, reg, &data, 1);

#elif defined(KX_PLATFORM_FIRMWARE)
	#ifdef USE_I2C_GPIO
	{
		unsigned char wb[2], rb[2];
		wb[0] = reg; wb[1] = data;
		i2c_write(KIONIX_ACCEL_I2C_SLV_ADDR, wb, 2);
	}
	#elif defined(USE_I2C_INT)
		Wr24C080((U32)KIONIX_ACCEL_I2C_SLV_ADDR, (U32)reg, data);
	#elif defined(USE_I2C_POLL)
		_Wr24C080((U32)KIONIX_ACCEL_I2C_SLV_ADDR, (U32)reg, data);
	#endif
#endif
	if(res < 0){
		//printf("Write failed on register 0x%02x with data 0x%02x\n", reg, data);
		KIONIX_LeaveCriticalSection(&tf9_crit_sect);
		return 1;
	}
	KIONIX_LeaveCriticalSection(&tf9_crit_sect);
	return res;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_get_device_type
DESCRIPTION:		This function return the Kionix accelerometer device type
ARGUMENTS PASSED:	None
RETURN VALUE:   	-1 = fail; 0: TE9; 1: TF9; 2: SD9
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	None
IMPORTANT NOTES:   	
==================================================================================================*/
int KIONIX_ACCEL_get_device_type(void)
{
	return device;
}

/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_deinit
DESCRIPTION:    	This function de-initializes the Kionix accelerometer. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	None
POST-CONDITIONS:   	Acceleration data outputs are disabled
==================================================================================================*/
int KIONIX_ACCEL_deinit(void)
{
	int res=0;

	res = KIONIX_ACCEL_disable_outputs();
	KIONIX_ACCEL_disable_interrupt();
	res |= KIONIX_ACCEL_disable_all();
	res |= KIONIX_ACCEL_sleep();
	KIONIX_DeleteCriticalSection(&tf9_crit_sect);

	return res;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_init
DESCRIPTION:    	This function initializes the Kionix accelerometer. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	None
POST-CONDITIONS:   	Acceleration data outputs are enabled
==================================================================================================*/
int KIONIX_ACCEL_init(void)
{
	int ctlreg_1 = 0;
	int ctlreg_3 = 0;
	int status = 0;
	char who_am_i = 0;
	long funcs = 0;
#ifdef KX_PLATFORM_LINUX
	//open i2c adapter; included checks for adapters 1, 2, and 3, but could be 0-255
#if 0
	fd = open("/dev/i2c-2", O_RDWR);
	if (fd == -1){
		fd = open("/dev/i2c-1", O_RDWR);
		if (fd == -1){
			fd = open("/dev/i2c-3", O_RDWR);
			if (fd == -1){		
				printf("Error opening adapter\n");
				exit(1);
			}
		}
	}
#else
	fd = open("/dev/i2c-0", O_RDWR);
	if (fd == -1){
		printf("Error opening adapter\n");
		return 1;
	}
#endif
	//set slave address for i2c adapter
	if (ioctl(fd, I2C_SLAVE, KIONIX_ACCEL_I2C_SLV_ADDR) < 0){
		printf("Failed to set slave address\n");
		exit(1);
	} 
	printf("set slave address success...\n");

	//i2c block functionality check
	if (ioctl(fd, I2C_FUNCS, &funcs) < 0){
		printf("I2C adapter failed functionality check\n");
	}
	printf("check functionality... ok\n");

	if (! (funcs & I2C_FUNC_SMBUS_READ_I2C_BLOCK)){
		printf("I2C block read function not available; using byte reads\n");
		block = 0;
	}
	else {
	   	printf("I2C Block read function success... using block reads\n");
	   	block = 1;
	}
	fflush(stdout);
#endif // #if defined(KX_PLATFORM_LINUX)
	KxMSleep(50); //wait 50ms
	KIONIX_InitializeCriticalSection(&tf9_crit_sect);

	//determine what hardware is included by reading the WHO_AM_I register
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_WHO_AM_I, &who_am_i, 1) == 0){
		switch(who_am_i){
			//KXTF9 initialization
			case 0x01:
			case 0x4E:
				KxPrint("[%d] found Device ID to(0x%x) \n", __LINE__, who_am_i );
				KxPrint("Initializing device: KXTF9\n");
				device = 1;

				status |= KXTF9_set_G_range(2);
				status |= KXTF9_set_resolution(12);
				status |= KXTF9_set_odr_tap(400);
				status |= KXTF9_set_odr_tilt(12);
				status |= KIONIX_ACCEL_set_odr_motion(50);
				status |= KIONIX_ACCEL_int_activeh();
				status |= KIONIX_ACCEL_int_latch();
				status |= KIONIX_ACCEL_disable_all();

				break;

			//KXTE9 initialization
			case 0x00:
				KxPrint("[%d] found Device ID to(0x%x) \n", __LINE__, who_am_i );
				KxPrint("Initializing device: KXTE9\n" );
				device = 0;
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_PC1);	/* disable accelerometer outputs */
				SET_REG_BIT(ctlreg_1, CTRL_REG1_ODRA);	/* set ODR to 40 Hz */
				SET_REG_BIT(ctlreg_1, CTRL_REG1_ODRB);

				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_TPE); /* disable the tilt position function */
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_WUFE);/* disable wake up function */
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_B2SE);/* disable back to sleep function */
				KIONIX_ACCEL_write_byte(0x3E, 0xCA);  	/* allow write access to threshold registers */
								 	/* this correction has been made in R1V6 ASIC */
				if (KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1) == 0){
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);	 
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);	/* set ODR to 40 Hz for B2S and WUF engines*/
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SB); 
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SA); 
					if (KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG3, ctlreg_3) == 0){
						if (KIONIX_ACCEL_tilt_timer(20) == 0){
							if (KIONIX_ACCEL_wuf_timer(4) == 0){
								if (KXTE9_b2s_timer(3) == 0){
									if (KIONIX_ACCEL_wuf_thresh(16) == 0){
										status = 0;
									}
									else	status = 1;
								}
								else	status = 1;
							}
							else	status = 1;
						}
						else	status = 1;
					}
					else	status = 1;
				}
				else	status = 1;
				KIONIX_ACCEL_write_byte(0x3E, 0x00);	/* block write access to threshold registers */
				break;
			default:
				KxPrint("[%d] found Device ID to(0x%x) \n", __LINE__, who_am_i );
				status = 1;				
				break;
		}
	}
	else {
		KxPrint("-E- can't reading the WHO_AM_I register\n");
		status = 1;
	}

	return status;
#if 0

	int ctlreg_1 = 0;
	int ctlreg_3 = 0;
	int status = 0;
	char who_am_i = 0;
	long funcs = 0;
	//open i2c adapter; included checks for adapters 1, 2, and 3, but could be 0-255
	fd = open("/dev/i2c-2", O_RDWR);
	if (fd == -1){
		fd = open("/dev/i2c-1", O_RDWR);
		if (fd == -1){
			fd = open("/dev/i2c-3", O_RDWR);
			if (fd == -1){		
				printf("Error opening adapter\n");
				exit(1);
			}
		}
	}
	//set slave address for i2c adapter
	if (ioctl(fd, I2C_SLAVE, KIONIX_ACCEL_I2C_SLV_ADDR) < 0){
		printf("Failed to set slave address\n");
		exit(1);
	}
	//i2c block functionality check
	if (ioctl(fd, I2C_FUNCS, &funcs) < 0){
		printf("I2C adapter failed functionality check\n");
	}
	if (! (funcs & I2C_FUNC_SMBUS_READ_I2C_BLOCK)){
		printf("I2C block read function not available; using byte reads\n");
		block = 0;
	}
	else 	block = 1;

	//determine what hardware is included by reading the WHO_AM_I register
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_WHO_AM_I, &who_am_i, 1) == 0){
		switch(who_am_i){
			//KXTF9 initialization
			case 0x4E:
				printf("Initializing device: KXTF9\n");
				device = 1;

				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_PC1);	/* disable accelerometer outputs */
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_RES); /* place accelerometer in 8-bit mode */
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_DRDYE); /* disable the availability of new acceleration data */
										/* to be reflected on the interrupt pin*/
				//set the sensor to +/-2g range
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_GSEL1); /* bit for setting g range */
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_GSEL0); /* bit for setting g range */
					
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_TPE); /* disable the tilt position function */
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_WUFE); /* disable wake up function */

				if (KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1) == 0){
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTA);	/* set ODR to 400 Hz for tap double tap function */
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTB); 
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OTPA); /* set ODR to 12.5 Hz for tilt position function */
					UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OTPB); 
					UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA); /* set ODR for general motion detection function */
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB); /* and high pass filtered outputs to 50 Hz */
					if (KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG3, ctlreg_3) == 0){
						//write Kionix recommended values for best performance
						if (KIONIX_ACCEL_tilt_timer(6) == 0){
							if (KIONIX_ACCEL_wuf_timer(4) == 0){
								
								status = 0;
							}
							else	status = 1;
						}
						else	status = 1;
					}
					else	status = 1;
				}
				else	status = 1;

				break;

			//KXTE9 initialization
			case 0x00:
				printf("Initializing device: KXTE9\n");
				device = 0;
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_PC1);	/* disable accelerometer outputs */
				SET_REG_BIT(ctlreg_1, CTRL_REG1_ODRA);	/* set ODR to 40 Hz */
				SET_REG_BIT(ctlreg_1, CTRL_REG1_ODRB);

				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_TPE); /* disable the tilt position function */
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_WUFE);/* disable wake up function */
				UNSET_REG_BIT(ctlreg_1, CTRL_REG1_B2SE);/* disable back to sleep function */
				KIONIX_ACCEL_write_byte(0x3E, 0xCA);  	/* allow write access to threshold registers */
								 	/* this correction has been made in R1V6 ASIC */
				if (KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1) == 0){
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);	 
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);	/* set ODR to 40 Hz for B2S and WUF engines*/
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SB); 
					SET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SA); 
					if (KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG3, ctlreg_3) == 0){
						if (KIONIX_ACCEL_tilt_timer(20) == 0){
							if (KIONIX_ACCEL_wuf_timer(4) == 0){
								if (KXTE9_b2s_timer(3) == 0){
									if (KIONIX_ACCEL_wuf_thresh(16) == 0){
										status = 0;
									}
									else	status = 1;
								}
								else	status = 1;
							}
							else	status = 1;
						}
						else	status = 1;
					}
					else	status = 1;
				}
				else	status = 1;
				KIONIX_ACCEL_write_byte(0x3E, 0x00);	/* block write access to threshold registers */
				break;
			default:
				status = 1;				
				break;
		}
	}
	else status = 1;
	KIONIX_ACCEL_enable_outputs();		/* enable acceleration data output */
	return status;
#endif
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_isr
DESCRIPTION:    	This function is the interrupt service routine for the accelerometer. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	None
PRE-CONDITIONS:   	None
POST-CONDITIONS:   	None
IMPORTANT NOTES:	Called from interrupt context, so do NOT do any i2c operations!
==================================================================================================*/
void KIONIX_ACCEL_isr(void)
{
#ifdef KX_PLATFORM_REX
	#if 0
	extern SU_TASK_HANDLE accel_task_handle;
	// disable accelerometer interrupt first
	KIONIX_ACCEL_disable_interrupt();
	// Set event to handle interrupt
	suSetEventMask(accel_task_handle, ACCEL_EVENT_INTERRUPT, NULL);
	// enable accelerometer interrupt again
	KIONIX_ACCEL_enable_interrupt();
	#else
	// disable accelerometer interrupt first
	KIONIX_ACCEL_disable_interrupt();
	// Set event to handle interrupt
	rex_set_sigs( &accel_tcb, SKY_ACCEL_ISR_SIG ); /* Signal a queue event */
	// TODO : Interrupt enable in SKY_ACCEL_ISR_SIG signal routine after process interrupt.
	#endif
#endif
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_enable_interrupt
DESCRIPTION:    	This function enables the interrupt for the accelerometer. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	None
PRE-CONDITIONS:   	None
POST-CONDITIONS:   	None
IMPORTANT NOTES:	Called from interrupt context, so do NOT do any i2c operations!
==================================================================================================*/
void KIONIX_ACCEL_enable_interrupt(void)
{
	// set up interrupt for rising edge detection
#ifdef KX_PLATFORM_REX
	// set up interrupt for rising edge detection
//	gpio_int_set_detect((gpio_int_type)ACCEL_GPIO_INT, DETECT_EDGE);
//	gpio_int_set_handler(ACCEL_GPIO_INT, ACTIVE_HIGH, (gpio_int_handler_type)KIONIX_ACCEL_isr);
	gpio_int_set_detect(BIO_ACC_INT, DETECT_EDGE);
	gpio_int_set_handler((gpio_int_type)BIO_ACC_INT, ACTIVE_HIGH, KIONIX_ACCEL_isr);
#endif
	KIONIX_ACCEL_enable_int();
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_disable_interrupt
DESCRIPTION:    	This function disables the interrupt for the accelerometer. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	None
PRE-CONDITIONS:   	None
POST-CONDITIONS:   	None
IMPORTANT NOTES:	Called from interrupt context, so do NOT do any i2c operations!
==================================================================================================*/
void KIONIX_ACCEL_disable_interrupt(void)
{
	KX_INTLOCK();

	KIONIX_ACCEL_disable_int();

#ifdef KX_PLATFORM_REX
//	gpio_int_set_handler(ACCEL_GPIO_INT, ACTIVE_HIGH, (gpio_int_handler_type)NULL);
	gpio_int_set_handler((gpio_int_type)BIO_ACC_INT, ACTIVE_HIGH, NULL);
#endif

	KX_INTFREE();
}

/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_enable_outputs
DESCRIPTION:    	This function enables accelerometer outputs. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Accelerometer outputs enabled
==================================================================================================*/
int KIONIX_ACCEL_enable_outputs(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		SET_REG_BIT(ctlreg_1, CTRL_REG1_PC1); /* sets PC1 bit to be in power up state */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_disable_outputs
DESCRIPTION:    	This function disables accelerometer outputs. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Accelerometer outputs disabled
==================================================================================================*/
int KIONIX_ACCEL_disable_outputs(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_PC1); /* sets PC1 bit to be in power up state */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_enable_tilt_function
DESCRIPTION:    	This function enables the tilt position function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Tilt position function is enabled
==================================================================================================*/
int KIONIX_ACCEL_enable_tilt_function(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		SET_REG_BIT(ctlreg_1, CTRL_REG1_TPE); /* sets TPE bit to enable tilt position function*/
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_disable_tilt_function
DESCRIPTION:    	This function disables the tilt position function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Tilt position function is disabled
==================================================================================================*/
int KIONIX_ACCEL_disable_tilt_function(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_TPE); /* unset TPE bit to disable tilt position function */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_enable_wake_up_function
DESCRIPTION:    	This function enables the wake up function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Wake up function is enabled
==================================================================================================*/
int KIONIX_ACCEL_enable_wake_up_function(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		SET_REG_BIT(ctlreg_1, CTRL_REG1_WUFE); /* set WUFE bit to enable the wake up function */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_disable_wake_up_function
DESCRIPTION:    	This function disables the wake up function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Wake up function is disabled
==================================================================================================*/
int KIONIX_ACCEL_disable_wake_up_function(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_WUFE); /* unset the WUFE bit to disable the wake up function */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_enable_all
DESCRIPTION:    	This function enables all engines. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	All engines enabled
==================================================================================================*/
int KIONIX_ACCEL_enable_all(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		SET_REG_BIT(ctlreg_1, CTRL_REG1_PC1); /* set PC1 to enable the accelerometer outputs */
		SET_REG_BIT(ctlreg_1, CTRL_REG1_TPE); /* set TPE bit to enable the tilt function */
		SET_REG_BIT(ctlreg_1, CTRL_REG1_WUFE); /* set WUFE to enable the wake up function */
		if (device == 0){	//KXTE9
			SET_REG_BIT(ctlreg_1, CTRL_REG1_B2SE); /* set B2SE to enable back to sleep function on KXTE9 */
		}
		else if (device == 1){	//KXTF9
			SET_REG_BIT(ctlreg_1, CTRL_REG1_TDTE); /* set TDTE to enable tap function on KXTF9 */
		}		
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_disable_all
DESCRIPTION:    	This function disables all engines. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	All engines disabled
==================================================================================================*/
int KIONIX_ACCEL_disable_all(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_PC1); /* unset the PC1 bit to disable the accelerometer outputs */
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_TPE); /* unset the TPE bit to disable the tilt function */
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_WUFE); /* unset WUFE to disable wake up function */
		if (device == 0){	//KXTE9
			UNSET_REG_BIT(ctlreg_1, CTRL_REG1_B2SE); /* set B2SE to enable back to sleep function on KXTE9 */
		}
		else if (device == 1){	//KXTF9
			UNSET_REG_BIT(ctlreg_1, CTRL_REG1_TDTE); /* set TDTE to enable tap function on KXTF9 */
		}
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_sleep
DESCRIPTION:    	This function places the accelerometer into a standby state while retaining 
			current register values. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Device is in sleep mode
==================================================================================================*/
int KIONIX_ACCEL_sleep(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0 ){
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_PC1); /* unset the PC1 bit to disable the accelerometer */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_interrupt_status
DESCRIPTION:    	This function reads the physical pin interrupt status. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = interrupt active; 1 = interrupt inactive
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	None
==================================================================================================*/
int KIONIX_ACCEL_read_interrupt_status(void)
{
	int interrupt_status; 
	char status_reg;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_STATUS_REG, &status_reg, 1) == 0){
		if ((status_reg & 0x10) == 0x00){
			interrupt_status = 1;
		}
		else	interrupt_status = 0;
	}
	else	interrupt_status = 1;
	return interrupt_status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_interrupt_source
DESCRIPTION:    	This function reads the Interrupt Source 2 register. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = interrupt active; 1 = interrupt inactive
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	interrupt_source contains the byte read from Interrupt Source Register 2
==================================================================================================*/
int KIONIX_ACCEL_read_interrupt_source(char* interrupt_source)
{
	int interrupt_status;
	if (device == 0){	//KXTE9
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_SRC_REG2, interrupt_source, 1) == 0){
			if (interrupt_source != 0x00){
				interrupt_status = 0;
			}
			else	interrupt_status = 1;
		}
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_SRC_REG2, interrupt_source, 1) == 0){
			if (interrupt_source != 0x00){
				interrupt_status = 0;
			}
			else	interrupt_status = 1;
		}
	}
	else	interrupt_status = 1;
	return interrupt_status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_previous_position
DESCRIPTION:    	This function reads the previous tilt position register. 
ARGUMENTS PASSED:   	previous_position pointer
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	previous_position is assigned
==================================================================================================*/
int KIONIX_ACCEL_read_previous_position(char* previous_position)
{
	int status;
	status = KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_TILT_POS_PRE, previous_position, 1);
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_current_position
DESCRIPTION:    	This function reads the current tilt position register. 
ARGUMENTS PASSED:   	current_position pointer
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	current_position is assigned
==================================================================================================*/
int KIONIX_ACCEL_read_current_position(char* current_position)
{
	int status;
	status = KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_TILT_POS_CUR, current_position, 1);
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_reset
DESCRIPTION:    	This function issues a software reset to the Kionix accelerometer. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Accelerometer is reset (will have to re-initialize)
==================================================================================================*/
int KIONIX_ACCEL_reset(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG3, &ctrl_reg3, 1) == 0){
		SET_REG_BIT(ctrl_reg3, CTRL_REG3_SRST);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_tilt_timer
DESCRIPTION:    	This function sets the number of tilt debounce samples. 
ARGUMENTS PASSED:   	tilt_timer; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Tile debounce set according to tilt_timer
==================================================================================================*/
int KIONIX_ACCEL_tilt_timer(int tilt_timer)
{
	int status;
	if (KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_TILT_TIMER, tilt_timer) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_wuf_timer
DESCRIPTION:    	This function sets the number of wake-up debounce samples. 
ARGUMENTS PASSED:   	wuf_timer; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Wake-up-function debounce set according to wuf_timer
==================================================================================================*/
int KIONIX_ACCEL_wuf_timer(int wuf_timer)
{
	int status;
	if (KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_WUF_TIMER, wuf_timer) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_wuf_thresh
DESCRIPTION:    	This function defines the threshold for general motion detection. 
ARGUMENTS PASSED:   	wuf_thresh; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Wake up function threshold set according to wuf_thresh
IMPORTANT NOTES:   	Default: 0.5g (0x20h)
==================================================================================================*/
int KIONIX_ACCEL_wuf_thresh(int wuf_thresh)
{
	int status;
	if (KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_WUF_THRESH, wuf_thresh) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_mask_z
DESCRIPTION:    	This function masks Z-axis from the activity engine. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Z-axis masked
==================================================================================================*/
int KIONIX_ACCEL_motion_mask_z(void)
{
	char int_ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_INT_CTRL_REG2, &int_ctrl_reg2, 1) == 0){
		SET_REG_BIT(int_ctrl_reg2, INT_CTRL_REG2_ZBW);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_INT_CTRL_REG2, int_ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_unmask_z
DESCRIPTION:    	This function unmasks Z-axis from the activity engine. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Z-axis unmasked
==================================================================================================*/
int KIONIX_ACCEL_motion_unmask_z(void)
{
	char int_ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_INT_CTRL_REG2, &int_ctrl_reg2, 1) == 0){
		UNSET_REG_BIT(int_ctrl_reg2, INT_CTRL_REG2_ZBW);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_INT_CTRL_REG2, int_ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_mask_y
DESCRIPTION:    	This function masks Y-axis from the activity engine. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Y-axis masked
==================================================================================================*/
int KIONIX_ACCEL_motion_mask_y(void)
{
	char int_ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_INT_CTRL_REG2, &int_ctrl_reg2, 1) == 0){
		SET_REG_BIT(int_ctrl_reg2, INT_CTRL_REG2_YBW);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_INT_CTRL_REG2, int_ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_unmask_y
DESCRIPTION:    	This function unmasks Y-axis from the activity engine. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Y-axis unmasked
==================================================================================================*/
int KIONIX_ACCEL_motion_unmask_y(void)
{
	char int_ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_INT_CTRL_REG2, &int_ctrl_reg2, 1) == 0){
		UNSET_REG_BIT(int_ctrl_reg2, INT_CTRL_REG2_YBW);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_INT_CTRL_REG2, int_ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_mask_x
DESCRIPTION:    	This function masks X-axis from the activity engine. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	X-axis masked
==================================================================================================*/
int KIONIX_ACCEL_motion_mask_x(void)
{
	char int_ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_INT_CTRL_REG2, &int_ctrl_reg2, 1) == 0){
		SET_REG_BIT(int_ctrl_reg2, INT_CTRL_REG2_XBW);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_INT_CTRL_REG2, int_ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_unmask_x
DESCRIPTION:    	This function unmasks X-axis from the activity engine. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	X-axis unmasked
==================================================================================================*/
int KIONIX_ACCEL_motion_unmask_x(void)
{
	char int_ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_INT_CTRL_REG2, &int_ctrl_reg2, 1) == 0){
		UNSET_REG_BIT(int_ctrl_reg2, INT_CTRL_REG2_XBW);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_INT_CTRL_REG2, int_ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_mask_fu
DESCRIPTION:    	This function masks face-up state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Face up state masked
==================================================================================================*/
int KIONIX_ACCEL_position_mask_fu(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		SET_REG_BIT(ctrl_reg2, CTRL_REG2_FUM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_mask_fd
DESCRIPTION:    	This function masks face-down state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Face down state masked
==================================================================================================*/
int KIONIX_ACCEL_position_mask_fd(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		SET_REG_BIT(ctrl_reg2, CTRL_REG2_FDM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_mask_up
DESCRIPTION:    	This function masks up state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Up state masked
==================================================================================================*/
int KIONIX_ACCEL_position_mask_up(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		SET_REG_BIT(ctrl_reg2, CTRL_REG2_UPM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_mask_do
DESCRIPTION:    	This function masks down state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Down state masked
==================================================================================================*/
int KIONIX_ACCEL_position_mask_do(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		SET_REG_BIT(ctrl_reg2, CTRL_REG2_DOM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_mask_ri
DESCRIPTION:    	This function masks right state in the screen rotation function.
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Right state masked
==================================================================================================*/
int KIONIX_ACCEL_position_mask_ri(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		SET_REG_BIT(ctrl_reg2, CTRL_REG2_RIM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_mask_le
DESCRIPTION:    	This function masks left state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Left state masked
==================================================================================================*/
int KIONIX_ACCEL_position_mask_le(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		SET_REG_BIT(ctrl_reg2, CTRL_REG2_LEM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_unmask_fu
DESCRIPTION:    	This function unmasks face-up state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Face up state unmasked
==================================================================================================*/
int KIONIX_ACCEL_position_unmask_fu(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		UNSET_REG_BIT(ctrl_reg2, CTRL_REG2_FUM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_unmask_fd
DESCRIPTION:    	This function unmasks face-down state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Face down state unmasked
==================================================================================================*/
int KIONIX_ACCEL_position_unmask_fd(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		UNSET_REG_BIT(ctrl_reg2, CTRL_REG2_FDM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_unmask_up
DESCRIPTION:    	This function unmasks up state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Up state unmasked
==================================================================================================*/
int KIONIX_ACCEL_position_unmask_up(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		UNSET_REG_BIT(ctrl_reg2, CTRL_REG2_UPM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_unmask_do
DESCRIPTION:    	This function unmasks down state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Down state unmasked
==================================================================================================*/
int KIONIX_ACCEL_position_unmask_do(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		UNSET_REG_BIT(ctrl_reg2, CTRL_REG2_DOM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_unmask_ri
DESCRIPTION:    	This function unmasks right state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Right state unmasked
==================================================================================================*/
int KIONIX_ACCEL_position_unmask_ri(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		UNSET_REG_BIT(ctrl_reg2, CTRL_REG2_RIM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_position_unmask_le
DESCRIPTION:    	This function unmasks left state in the screen rotation function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Left state unmasked
==================================================================================================*/
int KIONIX_ACCEL_position_unmask_le(void)
{
	char ctrl_reg2 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG2, &ctrl_reg2, 1) == 0){
		UNSET_REG_BIT(ctrl_reg2, CTRL_REG2_LEM);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG2, ctrl_reg2);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_set_odr_motion
DESCRIPTION:    	This function sets the ODR frequency.
ARGUMENTS PASSED:   	frequency variable; 1, 3, 10, or 40 for KXTE9; 25, 50, 100, or 200 for KXTF9
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	ODR is set according to frequency
==================================================================================================*/
int KIONIX_ACCEL_set_odr_motion(int frequency)
{
	char ctlreg_1 = 0;
	char ctlreg_3 = 0;	
	if (device == 0){	//KXTE9
		if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) != 0){
			return 1;
		} 
		if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG3, &ctlreg_3, 1) != 0){
			return 1;
		}
		switch (frequency){
		case 1:		/* set all ODR's to 1Hz */
			UNSET_REG_BIT(ctlreg_1, CTRL_REG1_ODRA);
			UNSET_REG_BIT(ctlreg_1, CTRL_REG1_ODRB);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SA);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SB);
			break;
		case 3:		/* set all ODR's to 3Hz */
			UNSET_REG_BIT(ctlreg_1, CTRL_REG1_ODRA);
			SET_REG_BIT(ctlreg_1, CTRL_REG1_ODRB);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SA);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SB);
			break;
		case 10:	/* set all ODR's to 10Hz */
			SET_REG_BIT(ctlreg_1, CTRL_REG1_ODRA);
			UNSET_REG_BIT(ctlreg_1, CTRL_REG1_ODRB);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SA);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SB);
			break;
		case 40:	/* set all ODR's to 40Hz */
			SET_REG_BIT(ctlreg_1, CTRL_REG1_ODRA);
			SET_REG_BIT(ctlreg_1, CTRL_REG1_ODRB);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SA);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OB2SB);
			break;
		default:
			return 1;
		}
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG3, ctlreg_3);
	}	
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG3, &ctlreg_3, 1) != 0){
			return 1;
		}
		switch (frequency){
		case 25:	/* set ODR to 25Hz */
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);
			break;
		case 50:	/* set ODR t0 50 Hz */
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);
			break;
		case 100:	/* set ODR to 100 Hz */
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);
			UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);
			break;
		case 200:	/* set ODR to 200 Hz */
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFA);
			SET_REG_BIT(ctlreg_3, CTRL_REG3_OWUFB);
			break;
		default:
			return 1;
		}
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG3, ctlreg_3);
	}
	return 0;
}

void KIONIX_ACCEL_process_directional_tap(char tap_mode, char tap_direction)
{
	switch(tap_direction)
	{
	case INT_CTRL_REG3_TFUM : // Z+
		KxPrint("Z+, (%d)  Back \n", tap_mode);
		break;
	case INT_CTRL_REG3_TFDM : // Z-
		KxPrint("Z-, (%d)  Front \n", tap_mode);
		break;
	case INT_CTRL_REG3_TUPM : // Y+
		KxPrint("Y+, (%d)  Down \n", tap_mode);
		break;
	case INT_CTRL_REG3_TDOM : // Y-
		KxPrint("Y-, (%d)  Up \n", tap_mode);
		break;
	case INT_CTRL_REG3_TRIM : // X+
		KxPrint("X+, (%d)  Left \n", tap_mode);
		break;
	case INT_CTRL_REG3_TLEM : // X-
		KxPrint("X-, (%d)  Right \n", tap_mode);
		break;
	}
	return;
}

void KIONIX_ACCEL_process_screen_rotation(char tilt_pos_pre, char tilt_pos_cur)
{
	if(tilt_pos_pre == tilt_pos_cur) {
		KxPrint("Tilt State is prev(0x%02x) == curr(0x%02x) \n", tilt_pos_pre, tilt_pos_cur);
		return;
	}

	switch(tilt_pos_cur)
	{
	case CTRL_REG2_RIM :  // X+
		KxPrint("X+,  Landscape LEFT \n");
		break;
	case CTRL_REG2_LEM :  // X-
		KxPrint("X-,  Landscape RIGHT \n");
		break;
	case CTRL_REG2_UPM : // Y+
		KxPrint("Y+,  Potrait UP \n");
		break;
	case CTRL_REG2_DOM : // Y-
		KxPrint("Y-,  Potrait DOWN \n");
		break;
	case CTRL_REG2_FUM : // Z+
		KxPrint("Z+,  Face UP \n");
		break;
	case CTRL_REG2_FDM : // Z-
		KxPrint("Z-,  Face DOWN \n");
		break;
	}

	return;
}

void KIONIX_ACCEL_process_motion_detection(char int_src_reg1, char int_src_reg2)
{
	switch(device)
	{
	case 0 : //KXTE9
		if(int_src_reg1 & 0x4) KxPrint("0x%02x, activity state has changed to Inactive \n", int_src_reg2); // B2SS
		if(int_src_reg1 & 0x2) KxPrint("0x%02x, activity state has changed to Active \n", int_src_reg2); // WUFS
		break;
	case 1 : // KXTF9
		KxPrint("TODO: TF9 motion detection process \n");
		break;
	case 2 : // KXSD9
		break;
	}

	return;
}

/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_service_interrupt
DESCRIPTION:    	This function clears the interrupt request status. 
ARGUMENTS PASSED:   	source_of_interrupt pointer
RETURN VALUE:   	0 = interrupt was pending; 1 = interrupt was not pending
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	source_of_interrupt is assigned
			Interrupt pending bit (MOTI in REGA) will be cleared
IMPORTANT NOTES:	Do not call this from interrupt context since it accesses i2c.
==================================================================================================*/
//int KIONIX_ACCEL_service_interrupt(int* source_of_interrupt)
int KIONIX_ACCEL_service_interrupt(void)
{
	char status_reg=0, int_rel=0, value=0;
	char int_src_reg1, int_src_reg2;
	char tilt_pos_cur, tilt_pos_pre;
	int res=0;

	KX_INTLOCK( );

	// disable accelerometer interrupt first
	KIONIX_ACCEL_disable_interrupt();

	KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_STATUS_REG, &status_reg, 1);
	KxPrint("-W- status_reg (0x%02x) \n", status_reg);
	if( !(status_reg & BIT(4))  ) {
		KxPrint("-W- no interrupt event (0x%02x) \n", status_reg);
		goto RELEASE_INT;
	}

	switch(device)
	{
	case 0: // KXTE9
		KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_SRC_REG1, &int_src_reg1, 1);
		KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_SRC_REG2, &int_src_reg2, 1);
		if( (int_src_reg1 & 0x4) || (int_src_reg1 & 0x2) ) { // B2SS or WUFS
			KIONIX_ACCEL_process_motion_detection(int_src_reg1, int_src_reg2);
		}
		if(int_src_reg1 & 0x1) { // TPS
			KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_TILT_POS_CUR, &tilt_pos_cur, 1);
			KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_TILT_POS_PRE, &tilt_pos_pre, 1);
	// TODO:
			KIONIX_ACCEL_process_screen_rotation(tilt_pos_pre, tilt_pos_cur);
		}
		break;
	case 1: // KXTF9
		KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_SRC_REG1, &int_src_reg1, 1);
		KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_SRC_REG2, &int_src_reg2, 1);
		if(int_src_reg2 & (0x3<<2) ) { // Direction tap
			unsigned char tap_mode ;
			tap_mode = ((int_src_reg2&(0x3<<2))>>2);
			KIONIX_ACCEL_process_directional_tap(tap_mode, int_src_reg1); // tap_mode(single/dobule), tap_direction
		}
		if(int_src_reg2 & BIT(0) ) { // TPS
			KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_TILT_POS_CUR, &tilt_pos_cur, 1);
			KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_TILT_POS_PRE, &tilt_pos_pre, 1);
			KIONIX_ACCEL_process_screen_rotation(tilt_pos_pre, tilt_pos_cur);
		}
		if(int_src_reg2 & BIT(1)) { // WUFS
			KIONIX_ACCEL_process_motion_detection(int_src_reg1, int_src_reg2);
		}
		break;
	case 2: // KXSD9
		break;
	}


RELEASE_INT:
	KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_INT_REL, &int_rel, 1);
	// enable accelerometer interrupt again
	KIONIX_ACCEL_enable_interrupt();

	KX_INTFREE( );

#if 0
	int return_status = 1;
	char dummy = 0;
	if (device == 0){	//KXTE9	
		// read the interrupt source register
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_SRC_REG2, (char *)source_of_interrupt, 1) == 0){
			// clear the interrupt source information along with interrupt pin
			if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_INT_REL, &dummy, 1) == 0){
				return_status = 0;
			}
			else	return_status = 1;
		}
		else	return_status = 1;
	}
	else if (device == 1){	//KXTF9
		// read the interrupt source register
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_SRC_REG2, (char *)source_of_interrupt, 1) == 0){
			// clear the interrupt source information along with interrupt pin
			if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_INT_REL, &dummy, 1) == 0){
				return_status = 0;
			}
			else	return_status = 1;
		}
		else	return_status = 1;
	}
	else return_status = 1;
	return return_status;
#endif // end of #if 0
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_LPF_cnt
DESCRIPTION:    	This function reads the number of counts on the X, Y, and Z axes.
ARGUMENTS PASSED:   	x, y, and z pointers
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	x, y, and z are assigned
==================================================================================================*/
int KIONIX_ACCEL_read_LPF_cnt(int* x, int* y, int* z)
{
	int status, x_sign, y_sign, z_sign;
	char Res, x_char;
	char ret[3] = {0, 0, 0};
	char xyz[6] = {0, 0, 0, 0, 0, 0};
	if (device == 0){	//KXTE9	
		status = KIONIX_ACCEL_read_bytes(KXTE9_I2C_XOUT, ret, 3);
		if(status == 0){
			*x = (int)(ret[0]) >> 2;
			*y = (int)(ret[1]) >> 2;
			*z = (int)(ret[2]) >> 2;
		}
		else status = 1;	
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &Res, 1) == 0){
			Res = Res & 0x40;
			switch(Res){
				case 0x00:	//low-resolution state
					if ((status = KIONIX_ACCEL_read_bytes(KXTF9_I2C_XOUT_L, xyz, 6)) == 0){
						*x = ((int)xyz[1]);
						x_sign = *x >> 7;	//1 = negative; 0 = positive
						if (x_sign == 1){
							*x = ((~(*x) + 0x01) & 0x0FF);
							*x = -(*x);
						}
						*y = ((int)xyz[3]);
						y_sign = *y >> 7;	//1 = negative; 0 = positive
						if (y_sign == 1){
							*y = ((~(*y) + 0x01) & 0x0FF);	//2's complement
							*y = -(*y);
						}
						*z = ((int)xyz[5]);
						z_sign = *z >> 7;	//1 = negative; 0 = positive
						if (z_sign == 1){
							*z = ((~(*z) + 0x01) & 0x0FF);	//2's complement
							*z = -(*z);						
						}
					}
					break;
				case 0x40:	//high-resolution state
					if ((status = KIONIX_ACCEL_read_bytes(KXTF9_I2C_XOUT_L, xyz, 6)) == 0){
						*x = ((int)xyz[0]) >> 4;
						*x = *x + (((int)xyz[1]) << 4);
						x_sign = *x >> 11; 	//1 = negative; 0 = positive
						if (x_sign == 1){
							*x = ((~(*x) + 0x01) & 0x0FFF);	//2's complement
							*x = -(*x);
						}
						*y = ((int)xyz[2]) >> 4;
						*y = *y + (((int)xyz[3]) << 4);
						y_sign = *y >> 11; 	//1 = negative; 0 = positive
						if (y_sign == 1){
							*y = ((~(*y) + 0x01) & 0x0FFF);	//2's complement
							*y = -(*y);
						}
						*z = ((int)xyz[4]) >> 4;
						*z = *z + (((int)xyz[5]) << 4);
						z_sign = *z >> 11; 	//1 = negative; 0 = positive
						if (z_sign == 1){
							*z = ((~(*z) + 0x01) & 0x0FFF);	//2's complement
							*z = -(*z);
						}
					}
					break;
			}		
		}
	}
	else status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_LPF_g
DESCRIPTION:    	This function reads the G(gravity force) values on the X, Y, and Z axes.
			The units used are milli-g's, or 1/1000*G. 
ARGUMENTS PASSED:   	gx, gy, and gz pointers
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	gx, gy, and gz are assigned
==================================================================================================*/
int KIONIX_ACCEL_read_LPF_g(int* gx, int* gy, int* gz)
{
	int status, sensitivity;
	int x = 0;
	int y = 0;
	int z = 0;
	int x_sign, y_sign, z_sign;
	char xyz[6] = {0, 0, 0, 0, 0, 0};
	char Res = 0;
	char G_range = 0;
	int range = 0;
	if (device == 0){	//KXTE9	
		sensitivity = BIT_SENSITIVITY_2_G;		
		if ((status = KIONIX_ACCEL_read_LPF_cnt(&x, &y, &z)) == 0){
			/* calculate milli-G's */
			*gx = 1000 * (x - ZERO_G_OFFSET) / sensitivity; 
			*gy = 1000 * (y - ZERO_G_OFFSET) / sensitivity; 
			*gz = 1000 * (z - ZERO_G_OFFSET) / sensitivity;
		}
		else	status = 1;
	}
	else if (device == 1){	//KXTF9
		//determine if in the low resolution or high resolution state
		if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &Res, 1) == 0){
			if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &G_range, 1) == 0){
				G_range = G_range & 0x18;
				G_range = G_range >> 3;
				switch(G_range){
					case 0:
						range = 2;
						break;
					case 1:
						range = 4;
						break;
					case 2:
						range = 8;
						break;
					default:
						break;
				}
				Res = Res & 0x40;
				switch(Res){
					case 0x00:	//low-resolution state
						if ((status = KIONIX_ACCEL_read_bytes(KXTF9_I2C_XOUT_L, xyz, 6)) == 0){
							x = ((int)xyz[1]);
							x_sign = x >> 7;	//1 = negative; 0 = positive
							if (x_sign == 1){
								x = ((~(x) + 0x01) & 0x0FF);
								x = -(x);
							}
							y = ((int)xyz[3]);
							y_sign = y >> 7;	//1 = negative; 0 = positive
							if (y_sign == 1){
								y = ((~(y) + 0x01) & 0x0FF);	//2's complement
								y = -(y);
							}
							z = ((int)xyz[5]);
							z_sign = z >> 7;	//1 = negative; 0 = positive
							if (z_sign == 1){
								z = ((~(z) + 0x01) & 0x0FF);	//2's complement
								z = -(z);						
							}
							sensitivity = (256)/(2*range);
							/* calculate milli-G's */
							*gx = 1000 * (x) / sensitivity; 
							*gy = 1000 * (y) / sensitivity; 
							*gz = 1000 * (z) / sensitivity;
						}
						break;
					case 0x40:	//high-resolution state
						if ((status = KIONIX_ACCEL_read_bytes(KXTF9_I2C_XOUT_L, xyz, 6)) == 0){
							x = ((int)xyz[0]) >> 4;
							x = x + (((int)xyz[1]) << 4);
							x_sign = x >> 11; 	//1 = negative; 0 = positive
							if (x_sign == 1){
								x = ((~(x) + 0x01) & 0x0FFF);	//2's complement
								x = -(x);
							}
							y = ((int)xyz[2]) >> 4;
							y = y + (((int)xyz[3]) << 4);
							y_sign = y >> 11;	//1 = negative; 0 = positive
							if (y_sign == 1){
								y = ((~(y) + 0x01) & 0x0FFF);	//2's complement
								y = -(y);
							}
							z = ((int)xyz[4]) >> 4;
							z = z + (((int)xyz[5]) << 4);
							z_sign = z >> 11;	//1 = negative; 0 = positive
							if (z_sign == 1){
								z = ((~(z) + 0x01) & 0x0FFF);	//2's complement
								z = -(z);
							}
							sensitivity = (4096)/(2*range);
							/* calculate milli-G's */
							*gx = 1000 * (x) / sensitivity; 
							*gy = 1000 * (y) / sensitivity; 
							*gz = 1000 * (z) / sensitivity;
						}
						break;
					default:
						break;
				}
			}
			else	status = 1;
			}
		else	status = 1;
		}
	else status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_current_odr_motion
DESCRIPTION:    	This function reads the current ODR of the general motion function. 
ARGUMENTS PASSED:   	ODR_rate_motion pointer
RETURN VALUE:   	0 = ODR set correctly; 1 = ODR invalid
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	ODR_rate_motion is assigned
==================================================================================================*/
int KIONIX_ACCEL_read_current_odr_motion(double* ODR_rate_motion)
{
	int status;
	char status_reg, ctrl_reg;
	if (device == 0){	//KXTE9	
		if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_STATUS_REG, &status_reg, 1) == 0){
			status_reg &= 0x0C;
			status_reg >>= 2;
			switch (status_reg){
			case 0:
				*ODR_rate_motion = 1;
				status = 0;
				break;
			case 1:
				*ODR_rate_motion = 3;
				status = 0;
				break;
			case 2:
				*ODR_rate_motion = 10;
				status = 0;
				break;
			case 3:
				*ODR_rate_motion = 40;
				status = 0;
				break;
			default:
				status = 1;
				break;
			}
		}
		else	status = 1;
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG3, &ctrl_reg, 1) == 0){
			ctrl_reg &= 0x03;
			ctrl_reg >>= 2;
			switch (ctrl_reg){
			case 0:
				*ODR_rate_motion = 25;
				status = 0;
				break;
			case 1:
				*ODR_rate_motion = 50;
				status = 0;
				break;
			case 2:
				*ODR_rate_motion = 100;
				status = 0;
				break;
			case 3:
				*ODR_rate_motion = 200;
				status = 0;
				break;
			default:
				status = 1;
				break;
			}
		}
		else	status = 1;
	}
	else status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_position_status
DESCRIPTION:    	This function reads INT_SRC_REG to determine if there was a change in tilt. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = tilt occurred; 1 = no tilt occurred
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	None
==================================================================================================*/
int KIONIX_ACCEL_read_position_status(void)
{
	int position_status; 
	char src_reg1, src_reg2;
	if (device == 0){	//KXTE9
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_SRC_REG1, &src_reg1, 1) == 0){
			if((src_reg1 & 0x01) == 1){
				position_status = 0;
			}
			else	position_status = 1;
		}
		else	position_status = 1;
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_SRC_REG2, &src_reg2, 1) == 0){
			if((src_reg2 & 0x01) == 1){
				position_status = 0;
			}
			else	position_status = 1;
		}
		else	position_status = 1;
	}
	else position_status = 1;
	return position_status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_read_wuf_status
DESCRIPTION:    	This function reads INT_SRC_REG1 to determine if wake up occurred. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = wake up occurred; 1 = no wake up occurred
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	None
==================================================================================================*/
int KIONIX_ACCEL_read_wuf_status(void)
{
	int wuf_status; 
	char src_reg1, src_reg2;
	if (device == 0){	//KXTE9	
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_SRC_REG1, &src_reg1, 1) == 0){
			if((src_reg1 & 0x02) == 0x02){
				wuf_status = 0;
			}
			else	wuf_status = 1;
		}
		else	wuf_status = 1;
	}
	else if (device == 1){	//KXTF9	
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_SRC_REG2, &src_reg2, 1) == 0){
			if((src_reg2 & 0x02) == 0x02){
				wuf_status = 0;
			}
			else	wuf_status = 1;
		}
		else	wuf_status = 1;
	}
	else wuf_status = 1;
	return wuf_status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_enable_int
DESCRIPTION:    	This function enables the physical interrupt. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Physical interrupt enabled
==================================================================================================*/
int KIONIX_ACCEL_enable_int(void)
{
	char int_ctrl_reg1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			SET_REG_BIT(int_ctrl_reg1, KXTE9_INT_CTRL_REG1_IEN);
			KIONIX_ACCEL_write_byte(KXTE9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			SET_REG_BIT(int_ctrl_reg1, KXTF9_INT_CTRL_REG1_IEN);
			KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_disable_int
DESCRIPTION:    	This function disables the physical interrupt. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Physical interrupt disabled
==================================================================================================*/
int KIONIX_ACCEL_disable_int(void)
{
	char int_ctrl_reg1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9	
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			UNSET_REG_BIT(int_ctrl_reg1, KXTE9_INT_CTRL_REG1_IEN);
			KIONIX_ACCEL_write_byte(KXTE9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			UNSET_REG_BIT(int_ctrl_reg1, KXTF9_INT_CTRL_REG1_IEN);
			KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else status = 1;	
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_int_activeh
DESCRIPTION:    	This function sets the polarity of physical interrupt pin to active high. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Physical interrupt set to active high
==================================================================================================*/
int KIONIX_ACCEL_int_activeh(void)
{
	char int_ctrl_reg1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9	
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			SET_REG_BIT(int_ctrl_reg1, KXTE9_INT_CTRL_REG1_IEA);
			KIONIX_ACCEL_write_byte(KXTE9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			SET_REG_BIT(int_ctrl_reg1, KXTF9_INT_CTRL_REG1_IEA);
			KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else status = 1;	
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_int_activel
DESCRIPTION:    	This function sets the polarity of physical interrupt pin to active low. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Physical interrupt set to active low
==================================================================================================*/
int KIONIX_ACCEL_int_activel(void)
{
	char int_ctrl_reg1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9	
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			UNSET_REG_BIT(int_ctrl_reg1, KXTE9_INT_CTRL_REG1_IEA);
			KIONIX_ACCEL_write_byte(KXTE9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			UNSET_REG_BIT(int_ctrl_reg1, KXTF9_INT_CTRL_REG1_IEA);
			KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else status = 1;	
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_int_latch
DESCRIPTION:    	This function sets the physical interrupt to a latch state. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Physical interrupt set to latched response
==================================================================================================*/
int KIONIX_ACCEL_int_latch(void)
{
	char int_ctrl_reg1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9	
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			UNSET_REG_BIT(int_ctrl_reg1, KXTE9_INT_CTRL_REG1_IEL);
			KIONIX_ACCEL_write_byte(KXTE9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			UNSET_REG_BIT(int_ctrl_reg1, KXTF9_INT_CTRL_REG1_IEL);
			KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else status = 1;	
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_int_pulse
DESCRIPTION:    	This function sets the physical interrupt to a pulse state. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Physical interrupt set to pulse response
==================================================================================================*/
int KIONIX_ACCEL_int_pulse(void)
{
	char int_ctrl_reg1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9	
		if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			SET_REG_BIT(int_ctrl_reg1, KXTE9_INT_CTRL_REG1_IEL);
			KIONIX_ACCEL_write_byte(KXTE9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else if (device == 1){	//KXTF9
		if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
			SET_REG_BIT(int_ctrl_reg1, KXTF9_INT_CTRL_REG1_IEL);
			KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
		}
		else	status = 1;
	}
	else status = 1;	
	return status;
}
/*==================================================================================================
					KXTE9-SPECIFIC FUNCTIONS
==================================================================================================*/
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_enable_back_to_sleep
DESCRIPTION:    	This function enables the back to sleep function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Back to sleep is enabled
==================================================================================================*/
int KIONIX_ACCEL_enable_back_to_sleep(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		SET_REG_BIT(ctlreg_1, CTRL_REG1_B2SE); /* set the B2SE bit to enable back to sleep function */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KIONIX_ACCEL_disable_back_to_sleep
DESCRIPTION:    	This function disables the back to sleep function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Back to sleep is disabled
==================================================================================================*/
int KIONIX_ACCEL_disable_back_to_sleep(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_B2SE); /* unset the B2SE bit to disable back to sleep function */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTE9_read_b2s_status
DESCRIPTION:    	This function reads INT_SRC_REG1 to determine if back to sleep occurred. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = back to sleep occurred; 1 = back to sleep did not occur
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	None
==================================================================================================*/
int KXTE9_read_b2s_status(void)
{
	int wuf_status; 
	char src_reg1;
	if (device == 1){	//KXTF9
		return 1;
	}	
	if (KIONIX_ACCEL_read_bytes(KXTE9_I2C_INT_SRC_REG1, &src_reg1, 1) == 0){
		if((src_reg1 & 0x04) == 0x04){
			wuf_status = 0;
		}
		else	wuf_status = 1;
	}
	else	wuf_status = 1;
	return wuf_status;
}
/*==================================================================================================
FUNCTION: 		KXTE9_b2s_timer
DESCRIPTION:    	This function sets the number of back-to-sleep debounce samples. 
ARGUMENTS PASSED:   	b2s_timer; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Back-to-sleep debounce set according to b2s_timer
==================================================================================================*/
int KXTE9_b2s_timer(int b2s_timer)
{
	int status;
	if (device == 1){	//KXTF9
		return 1;
	}
	if (KIONIX_ACCEL_write_byte(KXTE9_I2C_B2S_TIMER, b2s_timer) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTE9_b2s_thresh
DESCRIPTION:    	This function defines the threshold for back-to-sleep detection.
ARGUMENTS PASSED:   	b2s_thresh ; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Back-to-sleep function threshold set according to b2s_thresh
IMPORTANT NOTES:   	Default: 1.5g (0x60h)
==================================================================================================*/
int KXTE9_b2s_thresh(int b2s_thresh)
{
	int status;
	if (device == 1){	//KXTF9
		return 1;
	}
	if (KIONIX_ACCEL_write_byte(KXTE9_I2C_B2S_THRESH, b2s_thresh) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
					KXTF9-SPECIFIC FUNCTIONS
==================================================================================================*/
/*==================================================================================================
FUNCTION: 		KXTF9_enable_tap_detection
DESCRIPTION:    	This function enables the tap detection function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Tap detection is enabled
==================================================================================================*/
int KXTF9_enable_tap_detection(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		SET_REG_BIT(ctlreg_1, CTRL_REG1_TDTE); /* set TDTE bit to enable tap function */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_disable_tap_detection
DESCRIPTION:    	This function disables the tap detection function. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Tap detection is disabled
==================================================================================================*/
int KXTF9_disable_tap_detection(void)
{
	char ctlreg_1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) == 0){
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_TDTE); /* unset the TDTE bit to disable tap function */
		KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_read_single_tap_status
DESCRIPTION:    	This function reads INT_SRC_REG2 to determine whether a single tap event
			occurred. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = single tap occurred; 1 = single tap did not occur
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	None
==================================================================================================*/
int KXTF9_read_single_tap_status(void)
{
	int single_tap; 
	char src_reg2;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_SRC_REG2, &src_reg2, 1) == 0){
		if((src_reg2 & 0x0C) == 0x04){
			single_tap = 0;
		}
		else	single_tap = 1;
	}
	else	single_tap = 1;
	return single_tap;
}
/*==================================================================================================
FUNCTION: 		KXTF9_read_double_tap_status
DESCRIPTION:    	This function reads INT_SRC_REG2 to determine whether a double tap event
			occurred.
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = double tap occurred; 1 = double tap did not occur
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	None
==================================================================================================*/
int KXTF9_read_double_tap_status(void)
{
	int double_tap; 
	char src_reg2;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_SRC_REG2, &src_reg2, 1) == 0){
		if((src_reg2 & 0x0C) == 0x08){
			double_tap = 0;
		}
		else	double_tap = 1;
	}
	else	double_tap = 1;
	return double_tap;
}
/*==================================================================================================
FUNCTION: 		KXTF9_set_odr_tilt
DESCRIPTION:    	This function sets the ODR frequency for the tilt position function. 
ARGUMENTS PASSED:   	frequency variable; 1, 6, 12, or 50
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	ODR is set for tilt function according to frequency
==================================================================================================*/
int KXTF9_set_odr_tilt(int frequency)
{
	char ctlreg_3 = 0;
	if (device == 0){	//KXTE9
		// CTRL_REG1's initial output data rate ???
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG3, &ctlreg_3, 1) != 0){
		return 1;
	}
	switch (frequency){
	case 1:		/* set all ODR's to 1.6Hz */
		UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OTPA);
		UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OTPB);
		break;
	case 6:		/* set all ODR's to 6.3Hz */
		UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OTPA);
		SET_REG_BIT(ctlreg_3, CTRL_REG3_OTPB);
		break;
	case 12:	/* set all ODR's to 12.5Hz */
		SET_REG_BIT(ctlreg_3, CTRL_REG3_OTPA);
		UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OTPB);
		break;
	case 50:	/* set all ODR's to 50Hz */
		SET_REG_BIT(ctlreg_3, CTRL_REG3_OTPA);
		SET_REG_BIT(ctlreg_3, CTRL_REG3_OTPB);
		break;
	default:
		return 1;
	}
	KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG3, ctlreg_3);
	return 0;
}
/*==================================================================================================
FUNCTION: 		KXTF9_set_G_range
DESCRIPTION:    	This function sets the accelerometer G range. 
ARGUMENTS PASSED:   	range variable; 2, 4, or 8
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	G range is set according to range
==================================================================================================*/
int KXTF9_set_G_range(int range)
{
	char ctlreg_1 = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctlreg_1, 1) != 0){
		return 1;
	}
	switch (range){
	case 2:		/* set G-range to 2g */
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_GSEL1);
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_GSEL0);
		break;
	case 4:		/* set G-range to 4g */
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_GSEL1);
		SET_REG_BIT(ctlreg_1, CTRL_REG1_GSEL0);
		break;
	case 8:		/* set G-range to 8g */
		SET_REG_BIT(ctlreg_1, CTRL_REG1_GSEL1);
		UNSET_REG_BIT(ctlreg_1, CTRL_REG1_GSEL0);
		break;
	default:
		return 1;
	}
	KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG1, ctlreg_1);
	return 0;
}
/*==================================================================================================
FUNCTION: 		KXTF9_read_HPF_cnt
DESCRIPTION:    	This function reads the high pass filtered number of counts on the X, Y, 
			and Z axes. 
ARGUMENTS PASSED:   	x, y, and z pointers
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	x, y, and z are assigned
==================================================================================================*/
int KXTF9_read_HPF_cnt(int* x, int* y, int* z)
{
	int status, x_sign, y_sign, z_sign;
	char Res = 0;	
	char xyz[6] = {0, 0, 0, 0, 0, 0};
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &Res, 1) == 0){
			Res = Res & 0x40;
			switch(Res){
				case 0x00:	//low-resolution state
					if ((status = KIONIX_ACCEL_read_bytes(KXTF9_I2C_XOUT_HPF_L, xyz, 6)) == 0){
						*x = ((int)xyz[1]);
						x_sign = *x >> 7;	//1 = negative; 0 = positive
						if (x_sign == 1){
							*x = ((~(*x) + 0x01) & 0x0FF);
							*x = -(*x);
						}
						*y = ((int)xyz[3]);
						y_sign = *y >> 7;	//1 = negative; 0 = positive
						if (y_sign == 1){
							*y = ((~(*y) + 0x01) & 0x0FF);	//2's complement
							*y = -(*y);
						}
						*z = ((int)xyz[5]);
						z_sign = *z >> 7;	//1 = negative; 0 = positive
						if (z_sign == 1){
							*z = ((~(*z) + 0x01) & 0x0FF);	//2's complement
							*z = -(*z);						
						}
					}
					break;
				case 0x40:	//high-resolution state
					if ((status = KIONIX_ACCEL_read_bytes(KXTF9_I2C_XOUT_HPF_L, xyz, 6)) == 0){
						*x = ((int)xyz[0]) >> 4;
						*x = *x + (((int)xyz[1]) << 4);
						x_sign = *x >> 11; 	//1 = negative; 0 = positive
						if (x_sign == 1){
							*x = ((~(*x) + 0x01) & 0x0FFF);	//2's complement
							*x = -(*x);
						}
						*y = ((int)xyz[2]) >> 4;
						*y = *y + (((int)xyz[3]) << 4);
						y_sign = *y >> 11; 	//1 = negative; 0 = positive
						if (y_sign == 1){
							*y = ((~(*y) + 0x01) & 0x0FFF);	//2's complement
							*y = -(*y);
						}
						*z = ((int)xyz[4]) >> 4;
						*z = *z + (((int)xyz[5]) << 4);
						z_sign = *z >> 11; 	//1 = negative; 0 = positive
						if (z_sign == 1){
							*z = ((~(*z) + 0x01) & 0x0FFF);	//2's complement
							*z = -(*z);
						}
					}
					break;
			}		
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_read_HPF_g
DESCRIPTION:    	This function reads the G(gravity force) values on the X, Y, and Z axes.
			The units used are milli-g's, or 1/1000*G. 
ARGUMENTS PASSED:   	gx, gy, and gz pointers
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	gx, gy, and gz are assigned
==================================================================================================*/
int KXTF9_read_HPF_g(int* gx, int* gy, int* gz)
{
	int status;
	int x,y,z;
	int x_sign, y_sign, z_sign;
	int sensitivity;
	char xyz[6] = {0, 0, 0, 0, 0, 0};
	char Res = 0;
	char G_range = 0;
	int range = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	//determine if in the low resolution or high resolution state
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &Res, 1) == 0){
		if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &G_range, 1) == 0){
			G_range = G_range & 0x18;
			G_range = G_range >> 3;
			switch(G_range){
				case 0:
					range = 2;
					break;
				case 1:
					range = 4;
					break;
				case 2:
					range = 8;
					break;
				default:
					break;
			}
			Res = Res & 0x40; 
			switch(Res){
				case 0x00:	//low-resolution state
					if ((status = KIONIX_ACCEL_read_bytes(KXTF9_I2C_XOUT_HPF_L, xyz, 6)) == 0){
						x = ((int)xyz[1]);
						x_sign = x >> 7;	//1 = negative; 0 = positive
						if (x_sign == 1){
							x = ((~(x) + 0x01) & 0x0FF);
							x = -(x);
						}
						y = ((int)xyz[3]);
						y_sign = y >> 7;	//1 = negative; 0 = positive
						if (y_sign == 1){
							y = ((~(y) + 0x01) & 0x0FF);	//2's complement
							y = -(y);
						}
						z = ((int)xyz[5]);
						z_sign = z >> 7;	//1 = negative; 0 = positive
						if (z_sign == 1){
							z = ((~(z) + 0x01) & 0x0FF);	//2's complement
							z = -(z);						
						}						
						sensitivity = (256)/(2*range);
						/* calculate milli-G's */
						*gx = 1000 * (x) / sensitivity; 
						*gy = 1000 * (y) / sensitivity; 
						*gz = 1000 * (z) / sensitivity;
					}
					break;
				case 0x40:	//high-resolution state
					if ((status = KIONIX_ACCEL_read_bytes(KXTF9_I2C_XOUT_HPF_L, xyz, 6)) == 0){
						x = ((int)xyz[0]) >> 4;
						x = x + (((int)xyz[1]) << 4);
						x_sign = x >> 11; 	//1 = negative; 0 = positive
						if (x_sign == 1){
							x = ((~(x) + 0x01) & 0x0FFF);	//2's complement
							x = -(x);
						}
						y = ((int)xyz[2]) >> 4;
						y = y + (((int)xyz[3]) << 4);
						y_sign = y >> 11; 	//1 = negative; 0 = positive
						if (y_sign == 1){
							y = ((~(y) + 0x01) & 0x0FFF);	//2's complement
							y = -(y);
						}
						z = ((int)xyz[4]) >> 4;
						z = z + (((int)xyz[5]) << 4);
						z_sign = z >> 11; 	//1 = negative; 0 = positive
						if (z_sign == 1){
							z = ((~(z) + 0x01) & 0x0FFF);	//2's complement
							z = -(z);
						}
						sensitivity = (4096)/(2*range);
						/* calculate milli-G's */
						*gx = 1000 * (x) / sensitivity; 
						*gy = 1000 * (y) / sensitivity; 
						*gz = 1000 * (z) / sensitivity;
					}
					break;
				default:
					break;
			}
		}
		else	status = 1;
	}
	else status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_read_current_odr_tilt
DESCRIPTION:    	This function reads the current ODR of the tilt function. 
ARGUMENTS PASSED:   	ODR_rate_tilt pointer
RETURN VALUE:   	0 = ODR set correctly; 1 = ODR invalid
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	ODR_rate_tilt is assigned
==================================================================================================*/
int KXTF9_read_current_odr_tilt(double* ODR_rate_tilt)
{
	int status;
	char ctrl_reg;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG3, &ctrl_reg, 1) == 0){
		ctrl_reg &= 0x60;
		ctrl_reg >>= 5;
		switch (ctrl_reg){
		case 0:
			*ODR_rate_tilt = 1.6;
			status = 0;
			break;
		case 1:
			*ODR_rate_tilt = 6.3;
			status = 0;
			break;
		case 2:
			*ODR_rate_tilt = 12.5;
			status = 0;
			break;
		case 3:
			*ODR_rate_tilt = 50;
			status = 0;
			break;
		default:
			status = 1;
			break;
		}
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_read_current_odr_tap
DESCRIPTION:    	This function reads the current ODR of the tap double tap function. 
ARGUMENTS PASSED:   	ODR_rate_tap pointer
RETURN VALUE:   	0 = ODR set correctly; 1 = ODR invalid
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	ODR_rate_tap is assigned
==================================================================================================*/
int KXTF9_read_current_odr_tap(double* ODR_rate_tap)
{
	int status;
	char ctrl_reg;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG3, &ctrl_reg, 1) == 0){
		ctrl_reg &= 0x0C;
		ctrl_reg >>= 2;
		switch (ctrl_reg){
		case 0:
			*ODR_rate_tap = 50;
			status = 0;
			break;
		case 1:
			*ODR_rate_tap = 100;
			status = 0;
			break;
		case 2:
			*ODR_rate_tap = 200;
			status = 0;
			break;
		case 3:
			*ODR_rate_tap = 400;
			status = 0;
			break;
		default:
			status = 1;
			break;
		}
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_read_tap_direction
DESCRIPTION:    	This function reads INT_SRC_REG1 to determine which axis and in which direction
			a tap or double tap event occurred. 
ARGUMENTS PASSED:   	int_src_reg1 pointer
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	int_src_reg1 is assigned
==================================================================================================*/
int KXTF9_read_tap_direction(char* int_src_reg1)
{
	int status;
	if (device == 0){	//KXTE9
		return 1;
	}
	status = KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_SRC_REG1, int_src_reg1, 1);
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_int_alt_disable
DESCRIPTION:    	This function disables the alternate unlatched response for the physical 
			interrupt pin. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Alternate unlatched response for physical interrupt disabled
==================================================================================================*/
int KXTF9_int_alt_disable(void)
{
	char int_ctrl_reg1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
		UNSET_REG_BIT(int_ctrl_reg1, KXTF9_INT_CTRL_REG1_ULMB);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_int_alt_enable
DESCRIPTION:    	This function enables the alternate unlatched response for the physical 
			interrupt pin. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Alternate unlatched response for physical interrupt enabled
==================================================================================================*/
int KXTF9_int_alt_enable(void)
{
	char int_ctrl_reg1 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG1, &int_ctrl_reg1, 1) == 0){
		SET_REG_BIT(int_ctrl_reg1, KXTF9_INT_CTRL_REG1_ULMB);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG1, int_ctrl_reg1);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tdt_timer
DESCRIPTION:    	This function defines the minimum separation between the first and second
			taps in a double tap event. 
ARGUMENTS PASSED:   	tdt_timer; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Double tap minimum separation set according to tdt_timer
IMPORTANT NOTES:   	Default: 0.3s (0x78h)
==================================================================================================*/
int KXTF9_tdt_timer(int tdt_timer)
{
	int status;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_write_byte(KXTF9_I2C_TDT_TIMER, tdt_timer) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tdt_h_thresh
DESCRIPTION:    	This function defines the upper limit for the jerk threshold. 
ARGUMENTS PASSED:   	tdt_h_thresh; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Upper tap threshold set according to tdt_h_thresh
IMPORTANT NOTES:   	Default: 14j (0xB6h)
==================================================================================================*/
int KXTF9_tdt_h_thresh(int tdt_h_thresh)
{
	int status;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_write_byte(KXTF9_I2C_TDT_H_THRESH, tdt_h_thresh) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tdt_l_thresh
DESCRIPTION:    	This function defines the lower limit for the jerk threshold. 
ARGUMENTS PASSED:   	tdt_l_thresh; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Lower tap threshold set according to tdt_l_thresh
IMPORTANT NOTES:   	Default: 1j (0x1Ah)
==================================================================================================*/
int KXTF9_tdt_l_thresh(int tdt_l_thresh)
{
	int status;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_write_byte(KXTF9_I2C_TDT_L_THRESH, tdt_l_thresh) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tdt_tap_timer
DESCRIPTION:    	This function defines the minimum and maximum pulse width for the tap event. 
ARGUMENTS PASSED:   	tdt_tap_timer; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Tap timer set according to tdt_tap_timer
IMPORTANT NOTES:   	Default: 0.005s lower limit, 0.05s upper limit (0xA2h)
==================================================================================================*/
int KXTF9_tdt_tap_timer(int tdt_tap_timer)
{
	int status;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_write_byte(KXTF9_I2C_TDT_TAP_TIMER, tdt_tap_timer) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tdt_total_timer
DESCRIPTION:    	This function defines the amount of time that two taps in a double tap event 
			can be avove the PI threshold. 
ARGUMENTS PASSED:   	tdt_total_timer; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Total timer set according to tdt_total_timer
IMPORTANT NOTES:   	Default: 0.09s (0x24h)
==================================================================================================*/
int KXTF9_tdt_total_timer(int tdt_total_timer)
{
	int status;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_write_byte(KXTF9_I2C_TDT_TOTAL_TIMER, tdt_total_timer) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tdt_latency_timer
DESCRIPTION:    	This function defines the total amount of time that the tap algorithm will 
			count samples that are above the PI threshold. 
ARGUMENTS PASSED:   	tdt_latency_timer; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Latency timer set according to tdt_latency_timer
IMPORTANT NOTES:   	Default: 0.1s (0x28h)
==================================================================================================*/
int KXTF9_tdt_latency_timer(int tdt_latency_timer)
{
	int status;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_write_byte(KXTF9_I2C_TDT_LATENCY_TIMER, tdt_latency_timer) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tdt_window_timer
DESCRIPTION:    	This function defines the time window for the entire tap event, 
			single or double, to occur. 
ARGUMENTS PASSED:   	tdt_window_timer; 0-255
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Window timer set according to tdt_window_timer
IMPORTANT NOTES:   	Default: 0.4s (0xA0h)
==================================================================================================*/
int KXTF9_tdt_window_timer(int tdt_window_timer)
{
	int status;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_write_byte(KXTF9_I2C_TDT_WINDOW_TIMER, tdt_window_timer) == 0){
		status = 0;
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_mask_TFU
DESCRIPTION:    	This function masks Z+ (face up) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Z+ tap masked
==================================================================================================*/
int KXTF9_tap_mask_TFU(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		SET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TFUM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_unmask_TFU
DESCRIPTION:    	This function unmasks Z+ (face up) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Z+ tap unmasked
==================================================================================================*/
int KXTF9_tap_unmask_TFU(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		UNSET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TFUM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_mask_TFD
DESCRIPTION:    	This function masks Z- (face down) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Z- tap masked
==================================================================================================*/
int KXTF9_tap_mask_TFD(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		SET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TFDM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_unmask_TFD
DESCRIPTION:    	This function unmasks Z- (face down) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Z- tap unmasked
==================================================================================================*/
int KXTF9_tap_unmask_TFD(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		UNSET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TFDM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_mask_TUP
DESCRIPTION:    	This function masks Y+ (up) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Y+ tap masked
==================================================================================================*/
int KXTF9_tap_mask_TUP(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		SET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TUPM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_unmask_TUP
DESCRIPTION:    	This function unmasks Y+ (up) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Y+ tap unmasked
==================================================================================================*/
int KXTF9_tap_unmask_TUP(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		UNSET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TUPM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_mask_TDO
DESCRIPTION:    	This function masks Y- (down) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Y- tap masked
==================================================================================================*/
int KXTF9_tap_mask_TDO(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		SET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TDOM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_unmask_TDO
DESCRIPTION:    	This function unmasks Y- (down) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Y- tap unmasked
==================================================================================================*/
int KXTF9_tap_unmask_TDO(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		UNSET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TDOM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_mask_TRI
DESCRIPTION:    	This function masks X+ (right) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	X+ tap masked
==================================================================================================*/
int KXTF9_tap_mask_TRI(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		SET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TRIM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_unmask_TRI
DESCRIPTION:    	This function unmasks X+ (right) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	X+ tap unmasked
==================================================================================================*/
int KXTF9_tap_unmask_TRI(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		UNSET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TRIM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_mask_TLE
DESCRIPTION:    	This function masks X- (left) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	X- tap masked
==================================================================================================*/
int KXTF9_tap_mask_TLE(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		SET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TLEM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_unmask_TLE
DESCRIPTION:    	This function unmasks X- (left) tap. 
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	X- tap unmasked
==================================================================================================*/
int KXTF9_tap_unmask_TLE(void)
{
	char ctrl_reg3 = 0;
	int status = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_INT_CTRL_REG3, &ctrl_reg3, 1) == 0){
		UNSET_REG_BIT(ctrl_reg3, INT_CTRL_REG3_TLEM);
		KIONIX_ACCEL_write_byte(KXTF9_I2C_INT_CTRL_REG3, ctrl_reg3);
	}
	else	status = 1;
	return status;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_mask_all_direction
DESCRIPTION:    	
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	All direction tap masked
==================================================================================================*/
int KXTF9_tap_mask_all_direction(void)
{
	int res=0;

	res |= KXTF9_tap_mask_TFU(); // Z+
	res |= KXTF9_tap_mask_TFD(); // Z-
	res |= KXTF9_tap_mask_TUP(); // Y+
	res |= KXTF9_tap_mask_TDO(); // Y-
	res |= KXTF9_tap_mask_TRI(); // X+
	res |= KXTF9_tap_mask_TLE(); // X-

	return res;
}
/*==================================================================================================
FUNCTION: 		KXTF9_tap_unmask_all_direction
DESCRIPTION:    	
ARGUMENTS PASSED:   	None
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	All direction tap unmasked
==================================================================================================*/
int KXTF9_tap_unmask_all_direction(void)
{
	int res=0;

	res |= KXTF9_tap_unmask_TFU();  // Z+
	res |= KXTF9_tap_unmask_TFD(); // Z-
	res |= KXTF9_tap_unmask_TUP(); // Y+
	res |= KXTF9_tap_unmask_TDO(); // Y-
	res |= KXTF9_tap_unmask_TRI(); // X+
	res |= KXTF9_tap_unmask_TLE(); // X-

	return res;
}
/*==================================================================================================
FUNCTION: 		KXTF9_set_odr_tap
DESCRIPTION:    	This function sets the ODR frequency for the Direction Tap function. 
ARGUMENTS PASSED:   	frequency variable; 50, 100, 200 or 400
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	ODR is set for Direction Tap function according to frequency
==================================================================================================*/
int KXTF9_set_odr_tap(int frequency)
{
	char ctlreg_3 = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG3, &ctlreg_3, 1) != 0){
		return 1;
	}
	switch (frequency){
	case 50:		/* set all ODR's to 50Hz */
		UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTA);
		UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTB);
		break;
	case 100:		/* set all ODR's to 100Hz */
		UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTA);
		SET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTB);
		break;
	case 200:	/* set all ODR's to 200Hz */
		SET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTA);
		UNSET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTB);
		break;
	case 400:	/* set all ODR's to 400Hz */
		SET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTA);
		SET_REG_BIT(ctlreg_3, CTRL_REG3_OTDTB);
		break;
	default:
		return 1;
	}
	KIONIX_ACCEL_write_byte(KIONIX_ACCEL_I2C_CTRL_REG3, ctlreg_3);
	return 0;
}

/*==================================================================================================
FUNCTION: 		KXTF9_set_hpf_odr
DESCRIPTION:    	This function sets the high pass filter roll off frequency for the accelerometer outputs. 
ARGUMENTS PASSED:   	frequency, where roll_off_frequency = ODR/alpha; alpha = (50, 100, 200, 400)
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	High pass filter roll off is set according to frequency
==================================================================================================*/
int KXTF9_set_hpf_odr(int frequency)
{
	char data_ctrl_reg = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_DATA_CTRL_REG, &data_ctrl_reg, 1) != 0){
		return 1;
	}
	switch (frequency){
	case 50:	/* set tap ODR to 50Hz */
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_HPFROA);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_HPFROB);
		break;
	case 100:	/* set tap ODR to 100 Hz */
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_HPFROA);
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_HPFROB);
		break;
	case 200:	/* set tap ODR to 200 Hz */
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_HPFROA);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_HPFROB);
		break;
	case 400:	/* set tap ODR to 400 Hz */
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_HPFROA);
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_HPFROB);
		break;
	default:
		return 1;
	}
	KIONIX_ACCEL_write_byte(KXTF9_I2C_DATA_CTRL_REG, data_ctrl_reg);
	return 0;
}
/*==================================================================================================
FUNCTION: 		KXTF9_set_lpf_odr
DESCRIPTION:    	This function sets the low pass filter roll off for the accelerometer outputs. 
ARGUMENTS PASSED:   	roll off frequency (6, 12, 25, 50, 100, 200, 400)
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Low pass filter roll off is set according to frequency
==================================================================================================*/
int KXTF9_set_lpf_odr(int frequency)
{
	char data_ctrl_reg = 0;
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KXTF9_I2C_DATA_CTRL_REG, &data_ctrl_reg, 1) != 0){
		return 1;
	}
	switch (frequency){
	case 6:		/* set LPF rolloff to 6.25Hz */
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAA);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAB);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAC);
		break;
	case 12:	/* set LPF rolloff to 12.5Hz */
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAA);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAB);
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAC);
		break;
	case 25:	/* set LPF rolloff to 25Hz */
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAA);
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAB);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAC);
		break;
	case 50:	/* set LPF rolloff to 50Hz */
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAA);
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAB);
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAC);
		break;
	case 100:	/* set LPF rolloff to 100Hz */
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAA);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAB);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAC);
		break;
	case 200:	/* set LPF rolloff to 200 Hz */
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAA);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAB);
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAC);
		break;
	case 400:	/* set LPF rolloff to 400 Hz */
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAA);
		SET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAB);
		UNSET_REG_BIT(data_ctrl_reg, DATA_CTRL_REG_OSAC);
		break;
	default:
		return 1;
	}
	KIONIX_ACCEL_write_byte(KXTF9_I2C_DATA_CTRL_REG, data_ctrl_reg);
	return 0;
}
/*==================================================================================================
FUNCTION: 		KXTF9_set_resolution
DESCRIPTION:    	This function sets the resolution of the accelerometer outputs. 
ARGUMENTS PASSED:   	resolution (8-bit or 12-bit)
RETURN VALUE:   	0 = pass; 1 = fail
PRE-CONDITIONS:   	KIONIX_ACCEL_init() has been called
POST-CONDITIONS:   	Accelerometer resolution is set according to resolution
==================================================================================================*/
int KXTF9_set_resolution(int resolution)
{
	char ctrl_reg1 = 0;	
	if (device == 0){	//KXTE9
		return 1;
	}
	if (KIONIX_ACCEL_read_bytes(KIONIX_ACCEL_I2C_CTRL_REG1, &ctrl_reg1, 1) != 0){
		return 1;
	}
	switch (resolution){
	case 8:		/* set resolution to 8 bits */
		UNSET_REG_BIT(ctrl_reg1, CTRL_REG1_RES);		
		break;
	case 12:	/* set resolution to 12 bits */
		SET_REG_BIT(ctrl_reg1, CTRL_REG1_RES);
		break;
	default:
		return 1;
	}
	return 0;
}



//*************************************************************
//	KIONIX_SHAKE_Init
//		- initializes the shake detection engine
//	params
//		- shake_data* data = engine data
//	return
//		- none
//*************************************************************
void KIONIX_SHAKE_Init(shake_data* data)
{
    // init thresholds (convert ms to counts)
    data->maxDuration = CONFIG_DURATION / (1000 / CONFIG_RATE);
    data->maxDelay    = CONFIG_DELAY / (1000 / CONFIG_RATE);
    data->maxTimeout  = CONFIG_TIMEOUT / (1000 / CONFIG_RATE);

    // init timers
    data->cntDuration = 0;
    data->cntDelay    = 0;
    data->cntTimeout  = 0;

    // init counters
    data->cntShake    = 0;
    data->cntInvalid  = 0;
}


//*************************************************************
//	KIONIX_SHAKE_Update
//		- updates the shake detection engine
//      - maintains current shake count
//      - NOTE: must be called at a fixed interval
//	params
//		- shake_data* data = engine data
//      - long val = (x^2 + y^2 + z^2) / 1000
//	return
//		- long = current shake count
//*************************************************************
long KIONIX_SHAKE_Update(shake_data* data, long val)
{
    // possible shake...
    if (val > CONFIG_THRESHOLD)
    {
        // if the delay timer has started & 
        // not yet expired -> flag invalid
        if (data->cntDelay > 0)
            data->cntInvalid = 1;

        // inc duration
        data->cntDuration += 1;                
        
        // reset delay & timeout
        data->cntDelay   = data->maxDelay;
        data->cntTimeout = data->maxTimeout;
        
        return 0;
    }

    // shake detected...
    if ((data->cntDuration >= 2) && 
        (data->cntDuration <= data->maxDuration))
    {
        // add valid shakes to the count
        if (data->cntInvalid == 0)
            data->cntShake += 1;
    }    

    // flag valid & reset duration
    data->cntInvalid  = 0;    
    data->cntDuration = 0;

    // dec delay & timeout (if necessary)
    data->cntDelay   -= (data->cntDelay > 0 ? 1 : 0);
    data->cntTimeout -= (data->cntTimeout > 0 ? 1 : 0);
    
    // reset shake count after timeout
    if (data->cntTimeout <= 0)
    {
        data->cntShake   = 0;
        data->cntDelay   = 0;
        data->cntTimeout = 0;
    }

    return data->cntShake;
}


#ifdef __cplusplus
}
#endif
