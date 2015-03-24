  
#include "micro_esb.h"
#include "uesb_error_codes.h"

static uesb_payload_t tx_payload, rx_payload;

uint8_t rx_address_p0[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
uint8_t rx_address_p1[] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};

MicroESB microESB;

void uesb_event_handler()
{
    static uint32_t rf_interrupts;
    static uint32_t tx_attempts;
    
    microESB.uesb_get_clear_interrupts(&rf_interrupts);
    
    if(rf_interrupts & UESB_INT_TX_SUCCESS_MSK)
    {   
    }
    
    if(rf_interrupts & UESB_INT_TX_FAILED_MSK)
    {
        microESB.uesb_flush_tx();
    }
    
    if(rf_interrupts & UESB_INT_RX_DR_MSK)
    {
        microESB.uesb_read_rx_payload(&rx_payload);
        //NRF_GPIO->OUTCLR = 0xFUL << 8;
        //NRF_GPIO->OUTSET = (uint32_t)((rx_payload.data[2] & 0x0F) << 8);
    }
    
    microESB.uesb_get_tx_attempts(&tx_attempts);
//    NRF_GPIO->OUTCLR = 0xFUL << 12;
//    NRF_GPIO->OUTSET = (tx_attempts & 0x0F) << 12;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  
    Serial.println("1");
  
    uint8_t rx_addr_p0[] = {0x12, 0x34, 0x56, 0x78, 0x9A};
    uint8_t rx_addr_p1[] = {0xBC, 0xDE, 0xF0, 0x12, 0x23};
    uint8_t rx_addr_p2   = 0x66;
    
    //nrf_gpio_range_cfg_output(8, 15);
    
//    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
//    NRF_CLOCK->TASKS_HFCLKSTART = 1;
//    while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
    
    microESB.UESB_DEFAULT_CONFIG.mode                  = UESB_MODE_PTX;
    microESB.UESB_DEFAULT_CONFIG.protocol              = UESB_PROTOCOL_ESB_DPL;
    microESB.UESB_DEFAULT_CONFIG.event_handler         = 0;
    microESB.UESB_DEFAULT_CONFIG.rf_channel            = 2;
    microESB.UESB_DEFAULT_CONFIG.payload_length        = 61;
    microESB.UESB_DEFAULT_CONFIG.rf_addr_length        = 5;
    microESB.UESB_DEFAULT_CONFIG.bitrate               = UESB_BITRATE_2MBPS;
    microESB.UESB_DEFAULT_CONFIG.crc                   = UESB_CRC_16BIT;
    microESB.UESB_DEFAULT_CONFIG.tx_output_power       = UESB_TX_POWER_0DBM;
    //UESB_DEFAULT_CONFIG.rx_address_p0         = rx_address_p0;
    memcpy(microESB.UESB_DEFAULT_CONFIG.rx_address_p0, rx_address_p0, 5);
    //UESB_DEFAULT_CONFIG.rx_address_p1         = rx_address_p1;
    memcpy(microESB.UESB_DEFAULT_CONFIG.rx_address_p1, rx_address_p1, 5);
    
    microESB.UESB_DEFAULT_CONFIG.rx_address_p2         = 0xC3;
    microESB.UESB_DEFAULT_CONFIG.rx_address_p3         = 0xC4;
    microESB.UESB_DEFAULT_CONFIG.rx_address_p4         = 0xC5;
    microESB.UESB_DEFAULT_CONFIG.rx_address_p5         = 0xC6;
    microESB.UESB_DEFAULT_CONFIG.rx_address_p6         = 0xC7;
    microESB.UESB_DEFAULT_CONFIG.rx_address_p7         = 0xC8;
    microESB.UESB_DEFAULT_CONFIG.rx_pipes_enabled      = 0x3F;
    microESB.UESB_DEFAULT_CONFIG.dynamic_payload_length_enabled = 1;
    microESB.UESB_DEFAULT_CONFIG.dynamic_ack_enabled   = 0;
    microESB.UESB_DEFAULT_CONFIG.retransmit_delay      = 250;
    microESB.UESB_DEFAULT_CONFIG.retransmit_count      = 3;
    microESB.UESB_DEFAULT_CONFIG.tx_mode               = UESB_TXMODE_AUTO;
    microESB.UESB_DEFAULT_CONFIG.radio_irq_priority    = 1;
    
    Serial.println("UESB_DEFAULT_CONFIG");

    uesb_config_t uesb_config       = microESB.UESB_DEFAULT_CONFIG;
    uesb_config.rf_channel          = 5;
    uesb_config.crc                 = UESB_CRC_16BIT;
    uesb_config.retransmit_count    = 6;
    uesb_config.retransmit_delay    = 500;
    uesb_config.dynamic_ack_enabled = 0;
    uesb_config.protocol            = UESB_PROTOCOL_ESB_DPL;
    uesb_config.bitrate             = UESB_BITRATE_2MBPS;
    uesb_config.event_handler       = uesb_event_handler;
    
    microESB.uesb_init(&uesb_config);
    
    Serial.println("uesb_init");

    microESB.uesb_set_address(UESB_ADDRESS_PIPE0, rx_addr_p0);
    microESB.uesb_set_address(UESB_ADDRESS_PIPE1, rx_addr_p1);
    microESB.uesb_set_address(UESB_ADDRESS_PIPE2, &rx_addr_p2);

    tx_payload.length  = 8;
    tx_payload.pipe    = 0;
    tx_payload.data[0] = 0x01;
    tx_payload.data[1] = 0x00;
    tx_payload.data[2] = 0x00;
    tx_payload.data[3] = 0x00;
    tx_payload.data[4] = 0x11;
    
    Serial.println("payload set");
    
    while (true)
    {   
        Serial.println("writing ");
        
        int res = microESB.uesb_write_tx_payload(&tx_payload);
        
        if(res == UESB_SUCCESS)
        {
           Serial.println("UESB_SUCCESS ");
            tx_payload.data[1]++;
        } else if( res == UESB_ERROR_NOT_INITIALIZED) {
          Serial.println("UESB_ERROR_NOT_INITIALIZED ");
        } else if( res == UESB_ERROR_TX_FIFO_FULL) {
          Serial.println("UESB_ERROR_TX_FIFO_FULL ");
        }
        digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);              // wait for a second
        digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
        delay(1000);              // wait for a second
    }

}
