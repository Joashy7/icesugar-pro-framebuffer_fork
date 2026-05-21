module spi_peripheral # (
    parameter DATA_WIDTH=8, 
    parameter SPI_MODE = 0
) (
    input clk,
    input reset,
    input i_CS_n,
    input i_SCLK,
    input i_PICO,
    input i_tx_ready,
    input [DATA_WIDTH-1:0] i_tx_data,
    output reg o_rx_ready,
    output reg [DATA_WIDTH-1:0] o_rx_data,
    output o_POCI
);

reg [DATA_WIDTH-1:0] tx_data_reg;
`ifdef TEST_BENCH
reg [5:0] bit_index = DATA_WIDTH;
`else
reg [5:0] bit_index = DATA_WIDTH - 1;
`endif

wire CPHA;
wire CPOL;
wire phased_SCLK;

assign CPHA = SPI_MODE == 1 || SPI_MODE == 3;
assign CPOL = SPI_MODE == 2 || SPI_MODE == 3;

assign phased_SCLK = (CPHA) ? ~i_SCLK : i_SCLK;
assign o_POCI = 1'b0;

// clk clock domain. Process to save incoming tx data
always @(posedge clk) begin
    if(~reset & i_tx_ready) begin
        tx_data_reg <= i_tx_data;
    end
end

always @(posedge phased_SCLK or posedge i_CS_n) begin
    o_rx_ready <= 0;
    if(i_CS_n) begin
`ifdef TEST_BENCH
        bit_index = DATA_WIDTH;
`else
        bit_index = DATA_WIDTH - 1;
`endif
        o_rx_data <= 8'h00;
    end else begin
        o_rx_data <= {o_rx_data[DATA_WIDTH-2:0], i_PICO};
        if (bit_index == 0) begin
            o_rx_ready <= 1;
        end else begin
            bit_index <= bit_index - 1;
        end
    end
end

endmodule
