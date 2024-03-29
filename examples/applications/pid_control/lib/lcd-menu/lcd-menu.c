


struct menuitem
{
  const char *name; // name to be rendered
  const char * (*builderFunc)( const char *name );  // callback to generate string, if not null.
  functionPointer handlerFunc; // handler for this leaf node (optionally NULL)
  struct menu *child; // pointer to child submenu (optionally NULL)
};


struct menu
{
  struct menu *parent; // pointer to parent menu
  struct **menuitem; // array of menu items, NULL terminated
};



/* Forward-declare the menu structure, since it is used in the button structure. */
struct menu;

/* Button structure. */
struct button {
  const char *text;                   /* Text shown for the button. */
  const menu *next;                   /* Menu entered when button pressed, or NULL. */
  void      (*call)(unsigned char);   /* Function called when button pressed, or NULL. The parameter is the rate (speed). */
};

/* Menu structure. */
struct menu {
  const char    *text;                /* Descriptive text for this menu */
  void         (*draw)(menu *);       /* Function to draw this menu (values); menu in RAM */
  const button   up;                  /* Button marked 'up'. Usually action. */
  const button   down;                /* Button marked 'down'. Usually action. */
  const button   left;                /* Button marked 'left'. Usually menu. */
  const button   right;               /* Button marked 'right'. Usually menu. */
  const button   fine;                /* Fifth button, usually menu. */
};

/* Voltage and current changing functions */
void  voltage_change(int by)   { /* TODO */ }
void  current_change(int by)   { /* TODO */ }

/* Functions called (.call members in menu structures) when buttons are pressed. */
void  output_enable(unsigned char steps)       { /* TODO */ }
void  output_disable(unsigned char steps)      { /* TODO */ }
void  voltage_up_fine(unsigned char steps)     { voltage_change(     (int)steps ); }
void  voltage_down_fine(unsigned char steps)   { voltage_change(    -(int)steps ); }
void  voltage_up_coarse(unsigned char steps)   { voltage_change( +10*(int)steps ); }
void  voltage_down_coarse(unsigned char steps) { voltage_change( -10*(int)steps ); }
void  current_up_fine(unsigned char steps)     { current_change(     (int)steps ); }
void  current_down_fine(unsigned char steps)   { current_change(    -(int)steps ); }
void  current_up_coarse(unsigned char steps)   { current_change( +10*(int)steps ); }
void  current_down_coarse(unsigned char steps) { current_change( -10*(int)steps ); }

/* Functions to draw the display */
void  draw_output(const menu *curr)  { /* Draw menu 'curr' and current output state. */ }
void  draw_voltage(const menu *curr) { /* Draw menu 'curr' and the current voltage. */ }
void  draw_current(const menu *curr) { /* Draw menu 'curr' and the current current. */ }

/* Forward-declare all menus, so that they can refer to each other in any order. */
extern const menu  menu_output_fine     PROGMEM;  /* Main menu, fine adjust */
extern const menu  menu_voltage_fine    PROGMEM;  /* Fine voltage control */
extern const menu  menu_current_fine    PROGMEM;  /* Fine current control */
extern const menu  menu_output_coarse   PROGMEM;  /* Main menu, coarse adjust */
extern const menu  menu_voltage_coarse  PROGMEM;  /* Coarse voltage control */
extern const menu  menu_current_coarse  PROGMEM;  /* Coarse current control */

/* Define the menu structures. */

#define  menu_default  menu_output_coarse

const menu  menu_output_fine  PROGMEM = {
  .text  = "Output status",
  .draw  = draw_output,
  .up    = { .text = "On",      .next = NULL,                .call = output_enable  },
  .down  = { .text = "Off",     .next = NULL,                .call = output_disable },
  .left  = { .text = "Current", .next = &menu_current_fine,  .call = NULL           },
  .right = { .text = "Voltage", .next = &menu_voltage_fine,  .call = NULL           },
  .fine  = { .text = "",        .next = NULL,                .call = NULL           },
/*.fine  = { .text = "Coarse",  .next = &menu_output_coarse, .call = NULL           }, */
};

const menu  menu_voltage_fine  PROGMEM = {
  .text  = "Fine Voltage Control",
  .draw  = draw_voltage,
  .up    = { .text = "Increment", .next = NULL,                 .call = voltage_up_fine   },
  .down  = { .text = "Decrement", .next = NULL,                 .call = voltage_down_fine },
  .left  = { .text = "Output",    .next = &menu_output_fine,    .call = NULL              },
  .right = { .text = "Current",   .next = &menu_current_fine,   .call = NULL              },
  .fine  = { .text = "Coarse",    .next = &menu_voltage_coarse, .call = NULL              },
};

const menu  menu_current_fine  PROGMEM = {
  .text = "Fine Current Control",
  .draw  = draw_current,
  .up    = { .text = "Increment", .next = NULL,                 .call = current_up_fine   },
  .down  = { .text = "Decrement", .next = NULL,                 .call = current_down_fine },
  .left  = { .text = "Voltage",   .next = &menu_voltage_fine,   .call = NULL              },
  .right = { .text = "Output",    .next = &menu_output_fine,    .call = NULL              },
  .fine  = { .text = "Coarse",    .next = &menu_current_coarse, .call = NULL              },
};

const menu  menu_output_coarse  PROGMEM = {
  .text  = "Output Status",
  .draw  = draw_output,
  .up    = { .text = "On",      .next = NULL,                 .call = output_enable  },
  .down  = { .text = "Off",     .next = NULL,                 .call = output_disable },
  .left  = { .text = "Current", .next = &menu_current_coarse, .call = NULL           },
  .right = { .text = "Voltage", .next = &menu_voltage_coarse, .call = NULL           },
  .fine  = { .text = "",        .next = NULL,                 .call = NULL           },
/*.fine  = { .text = "Fine",    .next = &menu_output_fine,    .call = NULL           }, */
};

const menu  menu_voltage_coarse  PROGMEM = {
  .text  = "Coarse Voltage Control",
  .draw  = draw_voltage,
  .up    = { .text = "Increment", .next = NULL,                 .call = voltage_up_coarse   },
  .down  = { .text = "Decrement", .next = NULL,                 .call = voltage_down_coarse },
  .left  = { .text = "Output",    .next = &menu_output_coarse,  .call = NULL                },
  .right = { .text = "Current",   .next = &menu_current_coarse, .call = NULL                },
  .fine  = { .text = "Fine",      .next = &menu_voltage_fine,   .call = NULL                },
};

const menu  menu_current_coarse  PROGMEM = {
  .text  = "Coarse Current Control",
  .draw  = draw_current,
  .up    = { .text = "Increment", .next = NULL,                 .call = current_up_coarse   },
  .down  = { .text = "Decrement", .next = NULL,                 .call = current_down_coarse },
  .left  = { .text = "Voltage",   .next = &menu_voltage_coarse, .call = NULL                },
  .right = { .text = "Output",    .next = &menu_output_coarse,  .call = NULL                },
  .fine  = { .text = "Fine",      .next = &menu_current_fine,   .call = NULL                },
};

static const unsigned char  rate[] PROGMEM = {
  0,                          /* Released state causes no action! */
  1,                          /* Initial button press */
  0, 0, 0, 0, 0, 0, 0, 0, 0,  /* No action for nine display update cycles */
  1,                          /* First autorepeat */
  0, 0, 0, 0, 0, 0, 0, 0,     /* No action for eight display update cycles */
  1,                          /* Second autorepeat */
  0, 0, 0, 0, 0, 0, 0,        /* No action for seven display update cycles */
  1,                          /* Third autorepeat */
  0, 0, 0, 0, 0, 0,           /* No action for six display update cycles */
  1,                          /* Fourth autorepeat */
  0, 0, 0, 0, 0,
  1,
  0, 0, 0, 0,
  1,
  0, 0, 0,
  1,
  0, 0,
  1,
  0,                          /* Last delay */
  1,                          /* Final, BUTTONCOUNTER_MAX'th entry, repeats every display update cycle. */
};
#define  BUTTONCOUNTER_MAX  ((sizeof rate / sizeof rate[0]) - 1)

/* Button bits as a bit mask. */
#define  BUTTONMASK_NONE   0
#define  BUTTONMASK_UP     (1<<0)
#define  BUTTONMASK_DOWN   (1<<1)
#define  BUTTONMASK_LEFT   (1<<2)
#define  BUTTONMASK_RIGHT  (1<<3)
#define  BUTTONMASK_FINE   (1<<4)

/* Button state counters, supporting autorepeat and multiple buttons simultaneously. */
static unsigned char  buttoncounter_up;
static unsigned char  buttoncounter_down;
static unsigned char  buttoncounter_left;
static unsigned char  buttoncounter_right;
static unsigned char  buttoncounter_fine;

/* Current menu displayed. */
static const menu *currmenuref;
static menu        currmenu = {0};

void setup() {
  /* Clear button counters. */
  buttoncounter_up = 0;
  buttoncounter_down = 0;
  buttoncounter_left = 0;
  buttoncounter_right = 0;
  buttoncounter_fine = 0;
  /* Default menu */
  currmenuref = &(menu_default);
  memcpy_P(&currmenu, currmenuref, sizeof (menu));
  /* set button pins as inputs etc. */
}

/* Display update interval in milliseconds. */
#define  DISPLAY_UPDATE_MS  50

void loop() {
  /* Copy current menu structure to RAM. */
  memcpy_P(&currmenu, currmenuref, sizeof (menu));

  /* Update display. */
  currmenu.draw(&currmenu);

  /* Inner loop, checking for button presses.
   * This loops repeats for DISPLAY_UPDATE_MS milliseconds.
   */
  const unsigned long  now = millis();
  unsigned int         button_mask = 0;
  while ((unsigned long)(millis() - now) < DISPLAY_UPDATE_MS) {
    /* if (up pressed)    button_mask |= BUTTONMASK_UP;    */
    /* if (down pressed)  button_mask |= BUTTONMASK_DOWN;  */
    /* if (left pressed)  button_mask |= BUTTONMASK_LEFT;  */
    /* if (right pressed) button_mask |= BUTTONMASK_RIGHT; */
    /* if (fine pressed)  button_mask |= BUTTONMASK_FINE;  */

    /* Possibly other work, or maybe a delay(1); */

  }

  /* Update up button counter. */
  if (button_mask & BUTTONMASK_UP) {
    if (buttoncounter_up < BUTTONCOUNTER_MAX)
      buttoncounter_up++;
  } else
    buttoncounter_up = 0;

  /* Update down button counter. */
  if (button_mask & BUTTONMASK_DOWN) {
    if (buttoncounter_down < BUTTONCOUNTER_MAX)
      buttoncounter_down++;
  } else
    buttoncounter_down = 0;

  /* Update left button counter. */
  if (button_mask & BUTTONMASK_LEFT) {
    if (buttoncounter_left < BUTTONCOUNTER_MAX)
      buttoncounter_left++;
  } else
    buttoncounter_left = 0;

  /* Update right button counter. */
  if (button_mask & BUTTONMASK_RIGHT) {
    if (buttoncounter_right < BUTTONCOUNTER_MAX)
      buttoncounter_right++;
  } else
    buttoncounter_right = 0;

  /* Update fine button counter. */
  if (button_mask & BUTTONMASK_FINE) {
    if (buttoncounter_fine < BUTTONCOUNTER_MAX)
      buttoncounter_fine++;
  } else
    buttoncounter_fine = 0;

  /* Calculate button action rates. */
  const unsigned char  rate_up    = pgm_read_byte_near(rate + buttoncounter_up);
  const unsigned char  rate_down  = pgm_read_byte_near(rate + buttoncounter_down);
  const unsigned char  rate_left  = pgm_read_byte_near(rate + buttoncounter_left);
  const unsigned char  rate_right = pgm_read_byte_near(rate + buttoncounter_right);
  const unsigned char  rate_fine  = pgm_read_byte_near(rate + buttoncounter_fine);

  /* Apply actions first. */
  if (rate_up    && currmenu.up.call)    currmenu.up.call(rate_up);
  if (rate_down  && currmenu.down.call)  currmenu.down.call(rate_down);
  if (rate_left  && currmenu.left.call)  currmenu.left.call(rate_left);
  if (rate_right && currmenu.right.call) currmenu.right.call(rate_right);
  if (rate_fine  && currmenu.fine.call)  currmenu.fine.call(rate_fine);

  /* Menu change.  Menu changes only occur when only one button is pressed,
   * and only at the initial press time. This should be at the end of the loop() function.
  */
  if (buttoncounter_up == 1 && button_mask == BUTTONMASK_UP && currmenu.up.next)
    currmenuref = currmenu.up.next;
  else
  if (buttoncounter_down == 1 && button_mask == BUTTONMASK_DOWN && currmenu.down.next)
    currmenuref = currmenu.down.next;
  else
  if (buttoncounter_left == 1 && button_mask == BUTTONMASK_LEFT && currmenu.left.next)
    currmenuref = currmenu.left.next;
  else
  if (buttoncounter_right == 1 && button_mask == BUTTONMASK_RIGHT && currmenu.right.next)
    currmenuref = currmenu.right.next;
  else
  if (buttoncounter_fine == 1 && button_mask == BUTTONMASK_FINE && currmenu.fine.next)
    currmenuref = currmenu.fine.next;
}
