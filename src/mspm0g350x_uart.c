// For UART interrupt ringbuffers
static volatile uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t tx_buffer[UART_TX_BUFFER_SIZE];

static volatile uint32_t rx_read_index = 0;
static volatile uint32_t rx_write_index = 0;

static volatile uint32_t tx_read_index = 0;
static volatile uint32_t tx_write_index = 0;
