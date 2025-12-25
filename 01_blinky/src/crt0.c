int main(void);

void reset_handler(void)
{
    main();
    for (;;);
}
