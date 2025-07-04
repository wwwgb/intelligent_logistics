/* Includes ------------------------------------------------------------------*/
#include "bsp_hal_st7735s.h"
#include "bsp_lcd_font.h"
#include "spi.h"
/* Private macros ------------------------------------------------------------*/
#define LCD_SPI                          hspi1
#define LCD_CS_HIGH()                    HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_CS_LOW()                     HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin, GPIO_PIN_RESET)
#define LCD_RS_HIGH()                    HAL_GPIO_WritePin(LCD_A0_GPIO_Port,LCD_A0_Pin, GPIO_PIN_SET)
#define LCD_RS_LOW()                     HAL_GPIO_WritePin(LCD_A0_GPIO_Port,LCD_A0_Pin, GPIO_PIN_RESET)
#define LCD_RESET_HIGH()                 HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin, GPIO_PIN_SET)
#define LCD_RESET_LOW()                  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin, GPIO_PIN_RESET)
#define disp_spi_send_data(pdata, len)   HAL_SPI_Transmit(&LCD_SPI, pdata, len, len<<2)
#ifdef POWER_LCD_GPIO_Port
#define LCD_LED_ON()        HAL_GPIO_WritePin(POWER_LCD_GPIO_Port, POWER_LCD_Pin, GPIO_PIN_SET)
#define LCD_LED_OFF()       HAL_GPIO_WritePin(POWER_LCD_GPIO_Port, POWER_LCD_Pin, GPIO_PIN_RESET)
#else
#define LCD_LED_ON()    0
#define LCD_LED_OFF()   0
#endif
/* Private types -------------------------------------------------------------*/
typedef struct {
    uint8_t cmd;
    uint8_t data[20];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;
/* Private constants ---------------------------------------------------------*/
#if 1
const lcd_init_cmd_t init_cmds[]={
		{ST7735_SWRESET, {0}, 0x80},         		// Software reset, 0 args, w/delay 150
		{ST7735_SLPOUT,  {0}, 0x80},                 // Out of sleep mode, 0 args, w/delay 500
		{ST7735_FRMCTR1, {0x05, 0x3C, 0x3C}, 3},    // Frame rate ctrl - normal mode, 3 args: Rate = fosc/(1x2+40) * (LINE+2C+2D)
		{ST7735_FRMCTR2, {0x05, 0x3C, 0x3C}, 3},    // Frame rate control - idle mode, 3 args:Rate = fosc/(1x2+40) * (LINE+2C+2D)
		{ST7735_FRMCTR3, {0x05, 0x3C, 0x3C, 0x05, 0x3C, 0x3C}, 6}, //Frame rate ctrl - partial mode, 6 args:Dot inversion mode. Line inversion mode
		{ST7735_INVCTR,  {0x03}, 1},                 // Display inversion ctrl, 1 arg, no delay:No inversion
		{ST7735_PWCTR1,  {0x28, 0x08, 0x04}, 3},      // Power control, 3 args, no delay:-4.6V AUTO mode
		{ST7735_PWCTR2,  {0xC0}, 1},                 // Power control, 1 arg, no delay:VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
		{ST7735_PWCTR3,  {0x0D, 0x00}, 2},           // Power control, 2 args, no delay: Opamp current small, Boost frequency
		{ST7735_PWCTR4,  {0x8D, 0x2A}, 2},           // Power control, 2 args, no delay: BCLK/2, Opamp current small & Medium low
		{ST7735_PWCTR5,  {0x8D, 0xEE}, 2},           // Power control, 2 args, no delay:
		{ST7735_VMCTR1,  {0x1A}, 1},                 // Power control, 1 arg, no delay:
#if ST7735S_INVERT_COLORS == 1
		{ST7735_INVON, {0}, 0},                     // set inverted mode
#else
 		{ST7735_INVOFF, {0}, 0},                    // set non-inverted mode
#endif
		{ST7735_GMCTRP1, {0x04, 0x22, 0x07, 0x0A,
			              0x2E, 0x30, 0x25, 0x2A,
			              0x28, 0x26, 0x2E, 0x3A,
			              0x00, 0x01, 0x03, 0x13}, 16},           // 16 args, no delay:
		{ST7735_GMCTRN1, {0x04, 0x16, 0x06, 0x0D,
			              0x2D, 0x26, 0x23, 0x27,
			              0x27, 0x25, 0x2D, 0x3B,
			              0x00, 0x01, 0x04, 0x13}, 16},           // 16 args, no delay:
        {ST7735_COLMOD,  {0x05}, 1},               	// set color mode, 1 arg, no delay: 16-bit color
		{ST7735_DISPON,  {0}, TFT_INIT_DELAY},       // Main screen turn on, no args w/delay 100 ms delay
		{0, {0}, 0xff}
    };
#else
const lcd_init_cmd_t init_cmds[]={
		{ST7735_SWRESET, {0}, 0x80},         		// Software reset, 0 args, w/delay 150
		{ST7735_SLPOUT, {0}, 0x80},                 // Out of sleep mode, 0 args, w/delay 500
		{ST7735_FRMCTR1, {0x01, 0x2C, 0x2D}, 3},    // Frame rate ctrl - normal mode, 3 args: Rate = fosc/(1x2+40) * (LINE+2C+2D)
		{ST7735_FRMCTR2, {0x01, 0x2C, 0x2D}, 3},    // Frame rate control - idle mode, 3 args:Rate = fosc/(1x2+40) * (LINE+2C+2D)
		{ST7735_FRMCTR3, {0x01, 0x2C, 0x2D,0x01, 0x2C, 0x2D}, 6}, //Frame rate ctrl - partial mode, 6 args:Dot inversion mode. Line inversion mode
		{ST7735_INVCTR, {0x07}, 1},                 // Display inversion ctrl, 1 arg, no delay:No inversion
		{ST7735_PWCTR1, {0xA2,0x02, 0x84}, 3},      // Power control, 3 args, no delay:-4.6V AUTO mode
		{ST7735_PWCTR2, {0xC5}, 1},                 // Power control, 1 arg, no delay:VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
		{ST7735_PWCTR3, {0x0A, 0x00}, 2},           // Power control, 2 args, no delay: Opamp current small, Boost frequency
		{ST7735_PWCTR4, {0x8A,0x2A }, 2},           // Power control, 2 args, no delay: BCLK/2, Opamp current small & Medium low
		{ST7735_PWCTR5, {0x8A, 0xEE}, 2},           // Power control, 2 args, no delay:
		{ST7735_VMCTR1, {0x0E}, 1},                 // Power control, 1 arg, no delay:
#if ST7735S_INVERT_COLORS == 1
		{ST7735_INVON, {0}, 0},                     // set inverted mode
#else
 		{ST7735_INVOFF, {0}, 0},                    // set non-inverted mode
#endif
		{ST7735_COLMOD, {0x05}, 1},               	// set color mode, 1 arg, no delay: 16-bit color
		{ST7735_GMCTRP1, {0x02, 0x1c, 0x07, 0x12,
			0x37, 0x32, 0x29, 0x2d,
			0x29, 0x25, 0x2B, 0x39,
			0x00, 0x01, 0x03, 0x10}, 16},           // 16 args, no delay:
		{ST7735_GMCTRN1, {0x03, 0x1d, 0x07, 0x06,
			0x2E, 0x2C, 0x29, 0x2D,
			0x2E, 0x2E, 0x37, 0x3F,
			0x00, 0x00, 0x02, 0x10}, 16},           // 16 args, no delay:
		{ST7735_NORON, {0}, TFT_INIT_DELAY},       	// Normal display on, no args, w/delay 10 ms delay
		{ST7735_DISPON, {0}, TFT_INIT_DELAY},       // Main screen turn on, no args w/delay 100 ms delay
		{0, {0}, 0xff}
    };
#endif
/* Private variables ---------------------------------------------------------*/
uint16_t BACK_COLOR = BLUE;
uint16_t FORE_COLOR = WHITE;
/* Private functions ---------------------------------------------------------*/
static int8_t st7735s_send_cmd(uint8_t cmd);
static int8_t st7735s_send_data(void * data, uint16_t length);
static void st7735s_set_orientation(uint8_t orientation);
static void lcd_write_cmd(uint8_t cmd);
static void lcd_write_half_word(uint16_t da);
static void lcd_write_datas(uint8_t *data,uint32_t len );
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int st7735s_init(void)
{
	uint16_t cmd;
	//Reset the display
#ifdef LCD_RESET_GPIO_Port
	LCD_RESET_HIGH();
    My_mDelay(1);
	LCD_RESET_LOW();
    My_mDelay(10);
	LCD_RESET_HIGH();
    My_mDelay(20);
#endif
	//Send all the commands
    cmd = 0;
    while ( init_cmds[cmd].databytes != 0xff )
	{
        st7735s_send_cmd( init_cmds[cmd].cmd );
        st7735s_send_data( (uint8_t *)init_cmds[cmd].data, init_cmds[cmd].databytes&0x7F );
        if (init_cmds[cmd].databytes & 0x80) {
            My_mDelay(120);
        }
        cmd++;
    }
	st7735s_set_orientation(CONFIG_LCD_DISPLAY_ORIENTATION);
	lcd_clear(BACK_COLOR);
	return 0;
}
void lcd_address_set(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
	uint8_t data[4];
	
	/*Column addresses*/
    st7735s_send_cmd(0x2A);
    data[0] = (x_start >> 8) & 0xFF;
    data[1] = x_start & 0xFF;
    data[2] = (x_end >> 8) & 0xFF;
    data[3] = x_end & 0xFF;
    st7735s_send_data(data, 4);

    /*Page addresses*/
    st7735s_send_cmd(0x2B);
    data[0] = (y_start >> 8) & 0xFF;
    data[1] = y_start & 0xFF;
    data[2] = (y_end >> 8) & 0xFF;
    data[3] = y_end & 0xFF;
    st7735s_send_data(data, 4);
    st7735s_send_cmd(0x2C);      //开始写入GRAM
}
void st7735s_flush(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t *color)
{
	uint8_t buf[4] = {0};
	uint16_t height,width;
	
	width = x_end - x_start + 1;     
    height = y_end - y_start + 1;
	
	/*Column addresses*/
    st7735s_send_cmd(0x2A);
    buf[0] = (x_start >> 8) & 0xFF;
    buf[1] = x_start & 0xFF;
    buf[2] = (x_end >> 8) & 0xFF;
    buf[3] = x_end & 0xFF;
    st7735s_send_data(buf, 4);

    /*Page addresses*/
    st7735s_send_cmd(0x2B);
    buf[0] = (y_start >> 8) & 0xFF;
    buf[1] = y_start & 0xFF;
    buf[2] = (y_end >> 8) & 0xFF;
    buf[3] = y_end & 0xFF;
    st7735s_send_data(buf, 4);
    st7735s_send_cmd(0x2C);      //开始写入GRAM
	
	st7735s_send_data((uint8_t *)color,width*height*2);
}
void lcd_clear(uint16_t color)
{
	uint16_t i,j;
	uint8_t *buf = NULL;
	uint8_t color_buffer[2];
	
	color_buffer[0] = color>>8;
	color_buffer[1] = color;
    lcd_address_set(0, 0, ST7735S_WIDTH-1,ST7735S_HEIGHT-1);
	LCD_CS_LOW();
	LCD_RS_HIGH();
	buf = my_malloc(ST7735S_WIDTH*2);
	if(buf)
	{
		for (j = 0; j < ST7735S_WIDTH; j++)
		{
			buf[j * 2] =  color_buffer[0];
			buf[j * 2 + 1] =  color_buffer[1];
		}
		for (j=0; j<ST7735S_HEIGHT; j++)
		{
			disp_spi_send_data(buf,ST7735S_WIDTH*2);
		}
		my_free(buf);
	}
	else
	{
		for (j=0; j<ST7735S_HEIGHT; j++)
		{
			for (i=0; i<ST7735S_WIDTH; i++)
			{
				disp_spi_send_data( color_buffer, 2 );
			}
		}
	}
	LCD_CS_HIGH();
}
//开启显示
void lcd_display_on(void)
{
	lcd_write_cmd(0x11);  //sleep out and booster on 
	My_mDelay(20);    //延时120ms
	lcd_write_cmd(0x29); 
	My_mDelay(20);    //延时120ms
#ifdef POWER_LCD_GPIO_Port
	LCD_LED_ON();
#endif
}
//关闭显示
void lcd_display_off(void)
{
#ifdef POWER_LCD_GPIO_Port
	LCD_LED_OFF();
#endif
	lcd_write_cmd(0x10);  //sleep in and booster off	
	My_mDelay(20);    //延时120ms
	lcd_write_cmd(0x28);
	My_mDelay(20);    //延时120ms
}
/**
 * Set background color and foreground color
 *
 * @param   back    background color
 * @param   fore    fore color
 *
 * @return  void
 */
void lcd_set_color(uint16_t back, uint16_t fore)
{
    BACK_COLOR = back;
    FORE_COLOR = fore;
}
/**
 * display a point on the lcd.
 *
 * @param   x   x position
 * @param   y   y position
 *
 * @return  void
 */
void lcd_draw_point(uint16_t x, uint16_t y)
{
    lcd_address_set(x, y, x, y);
    lcd_write_half_word(FORE_COLOR);
}
/**
 * display a point on the lcd using the given colour.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   color   color of point
 *
 * @return  void
 */
void lcd_draw_point_color(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_address_set(x, y, x, y);
    lcd_write_half_word(color);
}
/**
 * full color on the lcd.
 *
 * @param   x_start     start of x position
 * @param   y_start     start of y position
 * @param   x_end       end of x position
 * @param   y_end       end of y position
 * @param   color       Fill color
 *
 * @return  void
 */
void lcd_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t i = 0, j = 0;
	
    lcd_address_set(x_start, y_start, x_end-1, y_end-1);

	for (i = y_start; i <= y_end; i++)
	{
		for (j = x_start; j <= x_end; j++)
		    lcd_write_half_word(color);
	}
}
/**
 * display a line on the lcd.
 *
 * @param   x1      x1 position
 * @param   y1      y1 position
 * @param   x2      x2 position
 * @param   y2      y2 position
 *
 * @return  void
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    uint32_t i = 0;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    uint8_t line_buf[LCD_W*2] = {0};
	
    if (y1 == y2)
    {
        /* fast draw transverse line */
        lcd_address_set(x1, y1, x2-1, y2-1);
        for (i = 0; i < x2 - x1; i++)
        {
            line_buf[2 * i] = FORE_COLOR >> 8;
            line_buf[2 * i + 1] = FORE_COLOR;
        }
        LCD_CS_LOW();
	    LCD_RS_HIGH();
        disp_spi_send_data(line_buf, (x2 - x1) * 2);
        LCD_CS_HIGH();
        return ;
    }

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    if (delta_x > 0)incx = 1;
    else if (delta_x == 0)incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)distance = delta_x;
    else distance = delta_y;
    for (t = 0; t <= distance + 1; t++)
    {
        lcd_draw_point(row, col);
        xerr += delta_x ;
        yerr += delta_y ;
        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}
/**
 * display a rectangle on the lcd.
 *
 * @param   x1      x1 position
 * @param   y1      y1 position
 * @param   x2      x2 position
 * @param   y2      y2 position
 *
 * @return  void
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_draw_line(x1, y1, x2, y1);
    lcd_draw_line(x1, y1, x1, y2);
    lcd_draw_line(x1, y2, x2, y2);
    lcd_draw_line(x2, y1, x2, y2);
}
/**
 * display a circle on the lcd.
 *
 * @param   x       x position of Center
 * @param   y       y position of Center
 * @param   r       radius
 *
 * @return  void
 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);
    while (a <= b)
    {
        lcd_draw_point(x0 - b, y0 - a);
        lcd_draw_point(x0 + b, y0 - a);
        lcd_draw_point(x0 - a, y0 + b);
        lcd_draw_point(x0 - b, y0 - a);
        lcd_draw_point(x0 - a, y0 - b);
        lcd_draw_point(x0 + b, y0 + a);
        lcd_draw_point(x0 + a, y0 - b);
        lcd_draw_point(x0 + a, y0 + b);
        lcd_draw_point(x0 - b, y0 + a);
        a++;
        //Bresenham
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
        lcd_draw_point(x0 + a, y0 + b);
    }
}
void lcd_show_char(uint16_t x, uint16_t y, uint8_t data, uint32_t size)
{
    uint8_t temp;
    uint16_t pos;
	uint8_t t;
    uint16_t colortemp = FORE_COLOR;
    uint8_t *font_buf = NULL;
	uint8_t num;

    if (x > LCD_W - size / 2 || y > LCD_H - size)return;

    data = data - ' ';
#ifdef ASC2_1206
	if (size == 12)
	{
		lcd_address_set(x, y, x + size / 2 - 1, y + size - 1);

		font_buf = (uint8_t *)malloc(size * size);
		if (!font_buf)
		{
			/* fast show char */
            for (pos = 0; pos < (size * 8) / 8; pos++)
            {
                temp = asc2_1206[(uint16_t)data * (size * 8) / 8 + pos];
                for (t = 0; t < 6; t++)
                {
                    if (temp & 0x80)colortemp = FORE_COLOR;
                    else colortemp = BACK_COLOR;
                    lcd_write_half_word(colortemp);
                    temp <<= 1;
                }
            }
		}
		else
		{
			for (pos = 0; pos < (size * 8) / 8; pos++)
            {
                temp = asc2_1206[(uint16_t)data * (size * 8) / 8 + pos];
                for (t = 0; t < 8; t++)
                {
                    if (temp & 0x80)colortemp = FORE_COLOR;
                    else colortemp = BACK_COLOR;
                    font_buf[2 * (6 * pos + t)] = colortemp >> 8;
                    font_buf[2 * (6 * pos + t) + 1] = colortemp;
                    temp <<= 1;
                }
            }
			lcd_write_datas(font_buf, size * size);
            free(font_buf);
		}
	}
	else
#endif
#ifdef ASC2_1608
    if (size == 16)
    {
        lcd_address_set(x, y, x + size / 2 - 1, y + size - 1);//(x,y,x+8-1,y+16-1)
		
        font_buf = (uint8_t *)malloc(size * size);
        if (!font_buf)
        {
            /* fast show char */
            for (pos = 0; pos < size * (size / 2) / 8; pos++)
            {
                temp = asc2_1608[(uint16_t)data * size * (size / 2) / 8 + pos];
                for (t = 0; t < 8; t++)
                {
                    if (temp & 0x80)colortemp = FORE_COLOR;
                    else colortemp = BACK_COLOR;
                    lcd_write_half_word(colortemp);
                    temp <<= 1;
                }
            }
        }
        else
        {
            for (pos = 0; pos < size * (size / 2) / 8; pos++)
            {
                temp = asc2_1608[(uint16_t)data * size * (size / 2) / 8 + pos];
                for (t = 0; t < 8; t++)
                {
                    if (temp & 0x80)colortemp = FORE_COLOR;
                    else colortemp = BACK_COLOR;
                    font_buf[2 * (8 * pos + t)] = colortemp >> 8;
                    font_buf[2 * (8 * pos + t) + 1] = colortemp;
                    temp <<= 1;
                }
            }
            lcd_write_datas(font_buf, size * size);
            free(font_buf);
        }
    }
    else
#endif

#ifdef ASC2_2412
	if (size == 24)
	{
		lcd_address_set(x, y, x + size / 2 - 1, y + size - 1);

		font_buf = (uint8_t *)malloc(size * size);
		if (!font_buf)
		{
			/* fast show char */
			for (pos = 0; pos < (size * 16) / 8; pos++)
			{
				temp = asc2_2412[(uint16_t)data * (size * 16) / 8 + pos];
				if (pos % 2 == 0)
				{
					num = 8;
				}
				else
				{
					num = 4;
				}

				for (t = 0; t < num; t++)
				{
					if (temp & 0x80)colortemp = FORE_COLOR;
					else colortemp = BACK_COLOR;
					lcd_write_half_word(colortemp);
					temp <<= 1;
				}
			}
		}
		else
		{
			for (pos = 0; pos < (size * 16) / 8; pos++)
			{
				temp = asc2_2412[(uint16_t)data * (size * 16) / 8 + pos];
				if (pos % 2 == 0)
				{
					num = 8;
				}
				else
				{
					num = 4;
				}

				for (t = 0; t < num; t++)
				{
					if (temp & 0x80)colortemp = FORE_COLOR;
					else colortemp = BACK_COLOR;
					if (num == 8)
					{
						font_buf[2 * (12 * (pos / 2) + t)] = colortemp >> 8;
						font_buf[2 * (12 * (pos / 2) + t) + 1] = colortemp;
					}
					else
					{
						font_buf[2 * (8 + 12 * (pos / 2) + t)] = colortemp >> 8;
						font_buf[2 * (8 + 12 * (pos / 2) + t) + 1] = colortemp;
					}
					temp <<= 1;
				}
			}
			lcd_write_datas(font_buf, size * size);
            free(font_buf);
		}
	}
	else
#endif

#ifdef ASC2_3216
	if (size == 32)
	{
		lcd_address_set(x, y, x + size / 2 - 1, y + size - 1);

		font_buf = (uint8_t *)malloc(size * size);
		if (!font_buf)
		{
			/* fast show char */
			for (pos = 0; pos < size * (size / 2) / 8; pos++)
			{
				temp = asc2_3216[(uint16_t)data * size * (size / 2) / 8 + pos];
				for (t = 0; t < 8; t++)
				{
					if (temp & 0x80)colortemp = FORE_COLOR;
					else colortemp = BACK_COLOR;
					lcd_write_half_word(colortemp);
					temp <<= 1;
				}
			}
		}
		else
		{
			for (pos = 0; pos < size * (size / 2) / 8; pos++)
			{
				temp = asc2_3216[(uint16_t)data * size * (size / 2) / 8 + pos];
				for (t = 0; t < 8; t++)
				{
					if (temp & 0x80)colortemp = FORE_COLOR;
					else colortemp = BACK_COLOR;
					font_buf[2 * (8 * pos + t)] = colortemp >> 8;
					font_buf[2 * (8 * pos + t) + 1] = colortemp;
					temp <<= 1;
				}
			}
			lcd_write_datas(font_buf, size * size);
            free(font_buf);
		}
	}
#endif

#ifdef ASC2_4824
	if (size == 48)
	{
		lcd_address_set(x, y, x + size / 2 - 1, y + size - 1);

		font_buf = (uint8_t *)malloc(size * size);
		if (!font_buf)
		{
			/* fast show char */
			for (pos = 0; pos < size * (size / 2) / 8; pos++)
			{
				temp = asc2_4824[(uint16_t)data * size * (size / 2) / 8 + pos];
				for (t = 0; t < 8; t++)
				{
					if (temp & 0x80)colortemp = FORE_COLOR;
					else colortemp = BACK_COLOR;
					lcd_write_half_word(colortemp);
					temp <<= 1;
				}
			}
		}
		else
		{
			for (pos = 0; pos < size * (size / 2) / 8; pos++)
			{
				temp = asc2_4824[(uint16_t)data * size * (size / 2) / 8 + pos];
				for (t = 0; t < 8; t++)
				{
					if (temp & 0x80)colortemp = FORE_COLOR;
					else colortemp = BACK_COLOR;
					font_buf[2 * (8 * pos + t)] = colortemp >> 8;
					font_buf[2 * (8 * pos + t) + 1] = colortemp;
					temp <<= 1;
				}
			}
			lcd_write_datas(font_buf, size * size);
            free(font_buf);
		}
	}
#endif
#ifdef ASC2_6432
	if (size == 64)
	{
		lcd_address_set(x, y, x + size / 2 - 1, y + size - 1);

		font_buf = (uint8_t *)malloc(size * size);
		if (!font_buf)
		{
			/* fast show char */
			for (pos = 0; pos < size * (size / 2) / 8; pos++)
			{
				temp = asc2_6432[(uint16_t)data * size * (size / 2) / 8 + pos];
				for (t = 0; t < 8; t++)
				{
					if (temp & 0x80)colortemp = FORE_COLOR;
					else colortemp = BACK_COLOR;
					lcd_write_half_word(colortemp);
					temp <<= 1;
				}
			}
		}
		else
		{
			for (pos = 0; pos < size * (size / 2) / 8; pos++)
			{
				temp = asc2_6432[(uint16_t)data * size * (size / 2) / 8 + pos];
				for (t = 0; t < 8; t++)
				{
					if (temp & 0x80)colortemp = FORE_COLOR;
					else colortemp = BACK_COLOR;
					font_buf[2 * (8 * pos + t)] = colortemp >> 8;
					font_buf[2 * (8 * pos + t) + 1] = colortemp;
					temp <<= 1;
				}
			}
			lcd_write_datas(font_buf, size * size);
            free(font_buf);
		}
	}
	else
#endif
	{
//		LOG("There is no any define ASC2_1208 && ASC2_2412 && ASC2_2416 && ASC2_3216 !");
	}
}
/**
 * display the string on the lcd.
 *
 * @param   x       x position
 * @param   y       y position
 * @param   size    size of font
 * @param   p       the string to be display
 *
 * @return   0: display success
 *          -1: size of font is not support
 */
int32_t lcd_show_string(uint16_t x, uint16_t y, uint32_t size, char *str)
{
    char *p = NULL;

    if ( (size != 12) &&
		 (size != 16) && 
	     (size != 24) && 
	     (size != 32) &&
	     (size != 48) &&
	     (size != 64) )
    {
        return -1;
    }
    p = str;
    while (*p != '\0')
    {
        if (x > LCD_W - size / 2)
        {
            x = 0;
            y += size;
        }
        if (y > LCD_H - size)
        {
            y = x = 0;
            lcd_clear(RED);
        }
        lcd_show_char(x, y, *p, size);
        x += size / 2;
        p++;
    }

    return 0;
}
//m^n函数
//返回值:m^n次方.
static uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	
	while(n--)result*=m;    
	return result;
}
void lcd_show_num(uint16_t x, uint16_t y,uint32_t size,uint32_t num,uint8_t len,uint8_t mode)
{
	uint8_t t,temp,enshow=0;
	
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode)
				{
					lcd_show_char(x+(size/2)*t, y, '0', size);
				} 
				else
				{	
                    lcd_show_char(x+(size/2)*t, y, ' ', size);				
				} 
 				continue;
			}else enshow=1;
		}
		lcd_show_char(x+(size/2)*t, y, temp+'0', size);
	}
}
void lcd_Display_Picture(uint16_t x, uint16_t y,uint16_t PicWidth,uint16_t PicHeight,const uint8_t *qImage)
{
	uint16_t colortemp;
	uint16_t pos;
	/* 设置图片的位置和大小， 即设置显示窗口 */
	lcd_address_set(x, y, x+PicWidth-1, y+PicHeight-1);
#if 1	
	for (pos = 0; pos < (PicWidth*PicHeight); pos++)
	{
		colortemp = (qImage[pos*2]<<8) | qImage[pos*2+1];
		if(colortemp == 0x0000)
		{
			colortemp = BACK_COLOR;
		}
		lcd_write_half_word(colortemp);
		
	}
#else
	lcd_write_datas((uint8_t*)qImage,PicWidth*PicHeight*2);
#endif
}
void lcd_Display_monochrome(uint16_t x, uint16_t y,uint16_t PicWidth,uint16_t PicHeight,const uint8_t *qImage)
{//显示单色图片
	uint8_t t,temp;
	uint16_t pos;
	uint16_t colortemp;
	
	lcd_address_set(x, y, x+PicWidth-1, y+PicHeight-1);
	
	/* fast show char */
	for (pos = 0; pos < (PicWidth*PicHeight/8); pos++)
	{
		temp = qImage[pos];
		for (t = 0; t < 8; t++)
		{
			if (temp & 0x80)colortemp = FORE_COLOR;
			else colortemp = BACK_COLOR;
			lcd_write_half_word(colortemp);
			temp <<= 1;
		}
	}
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
static void lcd_write_cmd(uint8_t cmd)
{
	LCD_CS_LOW();
    LCD_RS_LOW();
	disp_spi_send_data(&cmd, 1);
	LCD_CS_HIGH();
}
static void lcd_write_half_word(uint16_t da)
{//向液晶屏写一个16位数据
    uint8_t data[2] = {0};

    data[0] = da >> 8;
    data[1] = da;
	LCD_CS_LOW();
    LCD_RS_HIGH();
    disp_spi_send_data(data, 2);
	LCD_CS_HIGH();
}
static void lcd_write_datas(uint8_t *data,uint32_t len )
{
	LCD_CS_LOW();
	LCD_RS_HIGH();
	disp_spi_send_data(data, len);
	LCD_CS_HIGH();
}
static int8_t st7735s_send_cmd(uint8_t cmd)
{
	HAL_StatusTypeDef state;
	
	LCD_CS_LOW();
    LCD_RS_LOW();
	state = disp_spi_send_data(&cmd, 1);
	LCD_CS_HIGH();
    if ( state == HAL_OK )
		return 0;
	else
		return -1;
	
}
static int8_t st7735s_send_data(void * data, uint16_t length)
{
	HAL_StatusTypeDef state;
	
	LCD_CS_LOW();
	LCD_RS_HIGH();
	state = disp_spi_send_data(data, length);
	LCD_CS_HIGH();
    if ( state == HAL_OK )
		return 0;
	else
		return -1;
}
static void st7735s_set_orientation(uint8_t orientation)
{
	uint8_t data[4] = {0x79, 0xB9, 0xD9, 0x19};
	
	st7735s_send_cmd(0x36);
    st7735s_send_data((void *) &data[orientation], 1);
}










