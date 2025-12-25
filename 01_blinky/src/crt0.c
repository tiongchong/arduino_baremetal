void app_main(void);

void reset_handler(void)
{
    app_main();
    for (;;);
}
