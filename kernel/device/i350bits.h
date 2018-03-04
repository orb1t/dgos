// THIS FILE IS AUTOMATICALLY GENERATED
// from /home/doug/code/dgos/src/kernel/device/i350.bits

//
// I350_CTRL: Device control register

#define I350_CTRL_FD_BIT               0
#define I350_CTRL_GIOMD_BIT            2
#define I350_CTRL_SLU_BIT              6
#define I350_CTRL_ILOS_BIT             7
#define I350_CTRL_SPEED_BIT            8
#define I350_CTRL_FRCSPC_BIT           11
#define I350_CTRL_FRCDPLX_BIT          12
#define I350_CTRL_SDP0_GPIEN_BIT       16
#define I350_CTRL_SDP1_GPIEN_BIT       17
#define I350_CTRL_SDP0_DATA_BIT        18
#define I350_CTRL_SDP1_DATA_BIT        19
#define I350_CTRL_ADVD3WUC_BIT         20
#define I350_CTRL_SDP0_WDE_BIT         21
#define I350_CTRL_SDP0_IODIR_BIT       22
#define I350_CTRL_SDP1_IODIR_BIT       23
#define I350_CTRL_RST_BIT              26
#define I350_CTRL_RFCE_BIT             27
#define I350_CTRL_TFCE_BIT             28
#define I350_CTRL_DEV_RST_BIT          29
#define I350_CTRL_VME_BIT              30
#define I350_CTRL_PHY_RST_BIT          31

#define I350_CTRL_FD_BITS              1
#define I350_CTRL_GIOMD_BITS           1
#define I350_CTRL_SLU_BITS             1
#define I350_CTRL_ILOS_BITS            1
#define I350_CTRL_SPEED_BITS           2
#define I350_CTRL_FRCSPC_BITS          1
#define I350_CTRL_FRCDPLX_BITS         1
#define I350_CTRL_SDP0_GPIEN_BITS      1
#define I350_CTRL_SDP1_GPIEN_BITS      1
#define I350_CTRL_SDP0_DATA_BITS       1
#define I350_CTRL_SDP1_DATA_BITS       1
#define I350_CTRL_ADVD3WUC_BITS        1
#define I350_CTRL_SDP0_WDE_BITS        1
#define I350_CTRL_SDP0_IODIR_BITS      1
#define I350_CTRL_SDP1_IODIR_BITS      1
#define I350_CTRL_RST_BITS             1
#define I350_CTRL_RFCE_BITS            1
#define I350_CTRL_TFCE_BITS            1
#define I350_CTRL_DEV_RST_BITS         1
#define I350_CTRL_VME_BITS             1
#define I350_CTRL_PHY_RST_BITS         1
#define I350_CTRL_FD_MASK              ((1U << I350_CTRL_FD_BITS)-1)
#define I350_CTRL_GIOMD_MASK           ((1U << I350_CTRL_GIOMD_BITS)-1)
#define I350_CTRL_SLU_MASK             ((1U << I350_CTRL_SLU_BITS)-1)
#define I350_CTRL_ILOS_MASK            ((1U << I350_CTRL_ILOS_BITS)-1)
#define I350_CTRL_SPEED_MASK           ((1U << I350_CTRL_SPEED_BITS)-1)
#define I350_CTRL_FRCSPC_MASK          ((1U << I350_CTRL_FRCSPC_BITS)-1)
#define I350_CTRL_FRCDPLX_MASK         ((1U << I350_CTRL_FRCDPLX_BITS)-1)
#define I350_CTRL_SDP0_GPIEN_MASK      ((1U << I350_CTRL_SDP0_GPIEN_BITS)-1)
#define I350_CTRL_SDP1_GPIEN_MASK      ((1U << I350_CTRL_SDP1_GPIEN_BITS)-1)
#define I350_CTRL_SDP0_DATA_MASK       ((1U << I350_CTRL_SDP0_DATA_BITS)-1)
#define I350_CTRL_SDP1_DATA_MASK       ((1U << I350_CTRL_SDP1_DATA_BITS)-1)
#define I350_CTRL_ADVD3WUC_MASK        ((1U << I350_CTRL_ADVD3WUC_BITS)-1)
#define I350_CTRL_SDP0_WDE_MASK        ((1U << I350_CTRL_SDP0_WDE_BITS)-1)
#define I350_CTRL_SDP0_IODIR_MASK      ((1U << I350_CTRL_SDP0_IODIR_BITS)-1)
#define I350_CTRL_SDP1_IODIR_MASK      ((1U << I350_CTRL_SDP1_IODIR_BITS)-1)
#define I350_CTRL_RST_MASK             ((1U << I350_CTRL_RST_BITS)-1)
#define I350_CTRL_RFCE_MASK            ((1U << I350_CTRL_RFCE_BITS)-1)
#define I350_CTRL_TFCE_MASK            ((1U << I350_CTRL_TFCE_BITS)-1)
#define I350_CTRL_DEV_RST_MASK         ((1U << I350_CTRL_DEV_RST_BITS)-1)
#define I350_CTRL_VME_MASK             ((1U << I350_CTRL_VME_BITS)-1)
#define I350_CTRL_PHY_RST_MASK         ((1U << I350_CTRL_PHY_RST_BITS)-1)

// Full duplex (1=full)
#define I350_CTRL_FD                   (I350_CTRL_FD_MASK << I350_CTRL_FD_BIT)

// GIO master enable
#define I350_CTRL_GIOMD \
    (I350_CTRL_GIOMD_MASK << I350_CTRL_GIOMD_BIT)

// Set link up
#define I350_CTRL_SLU \
    (I350_CTRL_SLU_MASK << I350_CTRL_SLU_BIT)

// Invert loss-of-signal signal
#define I350_CTRL_ILOS \
    (I350_CTRL_ILOS_MASK << I350_CTRL_ILOS_BIT)

// (0=10, 1=100, 2=1000, 3=undefined) Mbps
#define I350_CTRL_SPEED \
    (I350_CTRL_SPEED_MASK << I350_CTRL_SPEED_BIT)

// Force speed
#define I350_CTRL_FRCSPC \
    (I350_CTRL_FRCSPC_MASK << I350_CTRL_FRCSPC_BIT)

// Force duplex
#define I350_CTRL_FRCDPLX \
    (I350_CTRL_FRCDPLX_MASK << I350_CTRL_FRCDPLX_BIT)

// General purpose interrupt detection enable for SDP0
#define I350_CTRL_SDP0_GPIEN \
    (I350_CTRL_SDP0_GPIEN_MASK << I350_CTRL_SDP0_GPIEN_BIT)

// General purpose interrupt detection enable for SDP0
#define I350_CTRL_SDP1_GPIEN \
    (I350_CTRL_SDP1_GPIEN_MASK << I350_CTRL_SDP1_GPIEN_BIT)

// SDP0 I/O pin data value
#define I350_CTRL_SDP0_DATA \
    (I350_CTRL_SDP0_DATA_MASK << I350_CTRL_SDP0_DATA_BIT)

// SDP1 I/O pin data value
#define I350_CTRL_SDP1_DATA \
    (I350_CTRL_SDP1_DATA_MASK << I350_CTRL_SDP1_DATA_BIT)

// D3Cold wakeup capability enable
#define I350_CTRL_ADVD3WUC \
    (I350_CTRL_ADVD3WUC_MASK << I350_CTRL_ADVD3WUC_BIT)

// Use SDP0 as watchdog
#define I350_CTRL_SDP0_WDE \
    (I350_CTRL_SDP0_WDE_MASK << I350_CTRL_SDP0_WDE_BIT)

// Data direction for SDP0
#define I350_CTRL_SDP0_IODIR \
    (I350_CTRL_SDP0_IODIR_MASK << I350_CTRL_SDP0_IODIR_BIT)

// Data direction for SDP1
#define I350_CTRL_SDP1_IODIR \
    (I350_CTRL_SDP1_IODIR_MASK << I350_CTRL_SDP1_IODIR_BIT)

// Software reset
#define I350_CTRL_RST \
    (I350_CTRL_RST_MASK << I350_CTRL_RST_BIT)

// Receive flow control enable
#define I350_CTRL_RFCE \
    (I350_CTRL_RFCE_MASK << I350_CTRL_RFCE_BIT)

// Transmit flow control enable
#define I350_CTRL_TFCE \
    (I350_CTRL_TFCE_MASK << I350_CTRL_TFCE_BIT)

// Device reset
#define I350_CTRL_DEV_RST \
    (I350_CTRL_DEV_RST_MASK << I350_CTRL_DEV_RST_BIT)

// VLAN mode enable
#define I350_CTRL_VME \
    (I350_CTRL_VME_MASK << I350_CTRL_VME_BIT)

// PHY reset
#define I350_CTRL_PHY_RST \
    (I350_CTRL_PHY_RST_MASK << I350_CTRL_PHY_RST_BIT)

#define I350_CTRL_FD_n(n)              ((n) << I350_CTRL_FD_BIT)
#define I350_CTRL_GIOMD_n(n)           ((n) << I350_CTRL_GIOMD_BIT)
#define I350_CTRL_SLU_n(n)             ((n) << I350_CTRL_SLU_BIT)
#define I350_CTRL_ILOS_n(n)            ((n) << I350_CTRL_ILOS_BIT)
#define I350_CTRL_SPEED_n(n)           ((n) << I350_CTRL_SPEED_BIT)
#define I350_CTRL_FRCSPC_n(n)          ((n) << I350_CTRL_FRCSPC_BIT)
#define I350_CTRL_FRCDPLX_n(n)         ((n) << I350_CTRL_FRCDPLX_BIT)
#define I350_CTRL_SDP0_GPIEN_n(n)      ((n) << I350_CTRL_SDP0_GPIEN_BIT)
#define I350_CTRL_SDP1_GPIEN_n(n)      ((n) << I350_CTRL_SDP1_GPIEN_BIT)
#define I350_CTRL_SDP0_DATA_n(n)       ((n) << I350_CTRL_SDP0_DATA_BIT)
#define I350_CTRL_SDP1_DATA_n(n)       ((n) << I350_CTRL_SDP1_DATA_BIT)
#define I350_CTRL_ADVD3WUC_n(n)        ((n) << I350_CTRL_ADVD3WUC_BIT)
#define I350_CTRL_SDP0_WDE_n(n)        ((n) << I350_CTRL_SDP0_WDE_BIT)
#define I350_CTRL_SDP0_IODIR_n(n)      ((n) << I350_CTRL_SDP0_IODIR_BIT)
#define I350_CTRL_SDP1_IODIR_n(n)      ((n) << I350_CTRL_SDP1_IODIR_BIT)
#define I350_CTRL_RST_n(n)             ((n) << I350_CTRL_RST_BIT)
#define I350_CTRL_RFCE_n(n)            ((n) << I350_CTRL_RFCE_BIT)
#define I350_CTRL_TFCE_n(n)            ((n) << I350_CTRL_TFCE_BIT)
#define I350_CTRL_DEV_RST_n(n)         ((n) << I350_CTRL_DEV_RST_BIT)
#define I350_CTRL_VME_n(n)             ((n) << I350_CTRL_VME_BIT)
#define I350_CTRL_PHY_RST_n(n)         ((n) << I350_CTRL_PHY_RST_BIT)

#define I350_CTRL_FD_GET(n) \
    (((n) & I350_CTRL_FD) >> I350_CTRL_FD_BIT)
#define I350_CTRL_GIOMD_GET(n) \
    (((n) & I350_CTRL_GIOMD) >> I350_CTRL_GIOMD_BIT)
#define I350_CTRL_SLU_GET(n) \
    (((n) & I350_CTRL_SLU) >> I350_CTRL_SLU_BIT)
#define I350_CTRL_ILOS_GET(n) \
    (((n) & I350_CTRL_ILOS) >> I350_CTRL_ILOS_BIT)
#define I350_CTRL_SPEED_GET(n) \
    (((n) & I350_CTRL_SPEED) >> I350_CTRL_SPEED_BIT)
#define I350_CTRL_FRCSPC_GET(n) \
    (((n) & I350_CTRL_FRCSPC) >> I350_CTRL_FRCSPC_BIT)
#define I350_CTRL_FRCDPLX_GET(n) \
    (((n) & I350_CTRL_FRCDPLX) >> I350_CTRL_FRCDPLX_BIT)
#define I350_CTRL_SDP0_GPIEN_GET(n) \
    (((n) & I350_CTRL_SDP0_GPIEN) >> I350_CTRL_SDP0_GPIEN_BIT)
#define I350_CTRL_SDP1_GPIEN_GET(n) \
    (((n) & I350_CTRL_SDP1_GPIEN) >> I350_CTRL_SDP1_GPIEN_BIT)
#define I350_CTRL_SDP0_DATA_GET(n) \
    (((n) & I350_CTRL_SDP0_DATA) >> I350_CTRL_SDP0_DATA_BIT)
#define I350_CTRL_SDP1_DATA_GET(n) \
    (((n) & I350_CTRL_SDP1_DATA) >> I350_CTRL_SDP1_DATA_BIT)
#define I350_CTRL_ADVD3WUC_GET(n) \
    (((n) & I350_CTRL_ADVD3WUC) >> I350_CTRL_ADVD3WUC_BIT)
#define I350_CTRL_SDP0_WDE_GET(n) \
    (((n) & I350_CTRL_SDP0_WDE) >> I350_CTRL_SDP0_WDE_BIT)
#define I350_CTRL_SDP0_IODIR_GET(n) \
    (((n) & I350_CTRL_SDP0_IODIR) >> I350_CTRL_SDP0_IODIR_BIT)
#define I350_CTRL_SDP1_IODIR_GET(n) \
    (((n) & I350_CTRL_SDP1_IODIR) >> I350_CTRL_SDP1_IODIR_BIT)
#define I350_CTRL_RST_GET(n) \
    (((n) & I350_CTRL_RST) >> I350_CTRL_RST_BIT)
#define I350_CTRL_RFCE_GET(n) \
    (((n) & I350_CTRL_RFCE) >> I350_CTRL_RFCE_BIT)
#define I350_CTRL_TFCE_GET(n) \
    (((n) & I350_CTRL_TFCE) >> I350_CTRL_TFCE_BIT)
#define I350_CTRL_DEV_RST_GET(n) \
    (((n) & I350_CTRL_DEV_RST) >> I350_CTRL_DEV_RST_BIT)
#define I350_CTRL_VME_GET(n) \
    (((n) & I350_CTRL_VME) >> I350_CTRL_VME_BIT)
#define I350_CTRL_PHY_RST_GET(n) \
    (((n) & I350_CTRL_PHY_RST) >> I350_CTRL_PHY_RST_BIT)

#define I350_CTRL_FD_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_FD) | I350_CTRL_FD_n((n)))
#define I350_CTRL_GIOMD_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_GIOMD) | I350_CTRL_GIOMD_n((n)))
#define I350_CTRL_SLU_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_SLU) | I350_CTRL_SLU_n((n)))
#define I350_CTRL_ILOS_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_ILOS) | I350_CTRL_ILOS_n((n)))
#define I350_CTRL_SPEED_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_SPEED) | I350_CTRL_SPEED_n((n)))
#define I350_CTRL_FRCSPC_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_FRCSPC) | I350_CTRL_FRCSPC_n((n)))
#define I350_CTRL_FRCDPLX_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_FRCDPLX) | I350_CTRL_FRCDPLX_n((n)))
#define I350_CTRL_SDP0_GPIEN_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_SDP0_GPIEN) | I350_CTRL_SDP0_GPIEN_n((n)))
#define I350_CTRL_SDP1_GPIEN_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_SDP1_GPIEN) | I350_CTRL_SDP1_GPIEN_n((n)))
#define I350_CTRL_SDP0_DATA_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_SDP0_DATA) | I350_CTRL_SDP0_DATA_n((n)))
#define I350_CTRL_SDP1_DATA_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_SDP1_DATA) | I350_CTRL_SDP1_DATA_n((n)))
#define I350_CTRL_ADVD3WUC_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_ADVD3WUC) | I350_CTRL_ADVD3WUC_n((n)))
#define I350_CTRL_SDP0_WDE_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_SDP0_WDE) | I350_CTRL_SDP0_WDE_n((n)))
#define I350_CTRL_SDP0_IODIR_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_SDP0_IODIR) | I350_CTRL_SDP0_IODIR_n((n)))
#define I350_CTRL_SDP1_IODIR_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_SDP1_IODIR) | I350_CTRL_SDP1_IODIR_n((n)))
#define I350_CTRL_RST_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_RST) | I350_CTRL_RST_n((n)))
#define I350_CTRL_RFCE_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_RFCE) | I350_CTRL_RFCE_n((n)))
#define I350_CTRL_TFCE_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_TFCE) | I350_CTRL_TFCE_n((n)))
#define I350_CTRL_DEV_RST_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_DEV_RST) | I350_CTRL_DEV_RST_n((n)))
#define I350_CTRL_VME_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_VME) | I350_CTRL_VME_n((n)))
#define I350_CTRL_PHY_RST_SET(r,n) \
    ((r) = ((r) & ~I350_CTRL_PHY_RST) | I350_CTRL_PHY_RST_n((n)))

//
// I350_STATUS: Device status register

#define I350_STATUS_FD_BIT                0
#define I350_STATUS_LU_BIT                1
#define I350_STATUS_LAN_ID_BIT            2
#define I350_STATUS_TXOFF_BIT             4
#define I350_STATUS_SPEED_BIT             6
#define I350_STATUS_ASDV_BIT              8
#define I350_STATUS_PHYRA_BIT             10
#define I350_STATUS_NUM_VFS_BIT           14
#define I350_STATUS_IOV_MODE_BIT          18
#define I350_STATUS_GIO_MES_BIT           19
#define I350_STATUS_DEV_RST_SET_BIT       20
#define I350_STATUS_PF_RST_DONE_BIT       21
#define I350_STATUS_MAC_CLK_GE_BIT        31

#define I350_STATUS_FD_BITS               1
#define I350_STATUS_LU_BITS               1
#define I350_STATUS_LAN_ID_BITS           2
#define I350_STATUS_TXOFF_BITS            1
#define I350_STATUS_SPEED_BITS            2
#define I350_STATUS_ASDV_BITS             2
#define I350_STATUS_PHYRA_BITS            1
#define I350_STATUS_NUM_VFS_BITS          4
#define I350_STATUS_IOV_MODE_BITS         1
#define I350_STATUS_GIO_MES_BITS          1
#define I350_STATUS_DEV_RST_SET_BITS      1
#define I350_STATUS_PF_RST_DONE_BITS      1
#define I350_STATUS_MAC_CLK_GE_BITS       1
#define I350_STATUS_FD_MASK               ((1U << I350_STATUS_FD_BITS)-1)
#define I350_STATUS_LU_MASK               ((1U << I350_STATUS_LU_BITS)-1)
#define I350_STATUS_LAN_ID_MASK           ((1U << I350_STATUS_LAN_ID_BITS)-1)
#define I350_STATUS_TXOFF_MASK            ((1U << I350_STATUS_TXOFF_BITS)-1)
#define I350_STATUS_SPEED_MASK            ((1U << I350_STATUS_SPEED_BITS)-1)
#define I350_STATUS_ASDV_MASK             ((1U << I350_STATUS_ASDV_BITS)-1)
#define I350_STATUS_PHYRA_MASK            ((1U << I350_STATUS_PHYRA_BITS)-1)
#define I350_STATUS_NUM_VFS_MASK          ((1U << I350_STATUS_NUM_VFS_BITS)-1)
#define I350_STATUS_IOV_MODE_MASK         ((1U << I350_STATUS_IOV_MODE_BITS)-1)
#define I350_STATUS_GIO_MES_MASK          ((1U << I350_STATUS_GIO_MES_BITS)-1)
#define I350_STATUS_DEV_RST_SET_MASK \
    ((1U << I350_STATUS_DEV_RST_SET_BITS)-1)
#define I350_STATUS_PF_RST_DONE_MASK \
    ((1U << I350_STATUS_PF_RST_DONE_BITS)-1)
#define I350_STATUS_MAC_CLK_GE_MASK \
    ((1U << I350_STATUS_MAC_CLK_GE_BITS)-1)

// Full duplex (1=full)
#define I350_STATUS_FD \
    (I350_STATUS_FD_MASK << I350_STATUS_FD_BIT)

// Link up (1=up)
#define I350_STATUS_LU \
    (I350_STATUS_LU_MASK << I350_STATUS_LU_BIT)

// LAN identifier for the MAC
#define I350_STATUS_LAN_ID \
    (I350_STATUS_LAN_ID_MASK << I350_STATUS_LAN_ID_BIT)

// Transmission paused due to flow control
#define I350_STATUS_TXOFF \
    (I350_STATUS_TXOFF_MASK << I350_STATUS_TXOFF_BIT)

// (0=10, 1=100, 2=1000, 3=undefined) Mbps
#define I350_STATUS_SPEED \
    (I350_STATUS_SPEED_MASK << I350_STATUS_SPEED_BIT)

// Auto-speed detection value
#define I350_STATUS_ASDV \
    (I350_STATUS_ASDV_MASK << I350_STATUS_ASDV_BIT)

// PHY reset asserted
#define I350_STATUS_PHYRA \
    (I350_STATUS_PHYRA_MASK << I350_STATUS_PHYRA_BIT)
#define I350_STATUS_NUM_VFS \
    (I350_STATUS_NUM_VFS_MASK << I350_STATUS_NUM_VFS_BIT)

// VF enable bit
#define I350_STATUS_IOV_MODE \
    (I350_STATUS_IOV_MODE_MASK << I350_STATUS_IOV_MODE_BIT)

// GIO master enable status
#define I350_STATUS_GIO_MES \
    (I350_STATUS_GIO_MES_MASK << I350_STATUS_GIO_MES_BIT)

// Device reset set
#define I350_STATUS_DEV_RST_SET \
    (I350_STATUS_DEV_RST_SET_MASK << I350_STATUS_DEV_RST_SET_BIT)

// Device reset completed
#define I350_STATUS_PF_RST_DONE \
    (I350_STATUS_PF_RST_DONE_MASK << I350_STATUS_PF_RST_DONE_BIT)

// MAC clock gating enabled
#define I350_STATUS_MAC_CLK_GE \
    (I350_STATUS_MAC_CLK_GE_MASK << I350_STATUS_MAC_CLK_GE_BIT)

#define I350_STATUS_FD_n(n)               ((n) << I350_STATUS_FD_BIT)
#define I350_STATUS_LU_n(n)               ((n) << I350_STATUS_LU_BIT)
#define I350_STATUS_LAN_ID_n(n)           ((n) << I350_STATUS_LAN_ID_BIT)
#define I350_STATUS_TXOFF_n(n)            ((n) << I350_STATUS_TXOFF_BIT)
#define I350_STATUS_SPEED_n(n)            ((n) << I350_STATUS_SPEED_BIT)
#define I350_STATUS_ASDV_n(n)             ((n) << I350_STATUS_ASDV_BIT)
#define I350_STATUS_PHYRA_n(n)            ((n) << I350_STATUS_PHYRA_BIT)
#define I350_STATUS_NUM_VFS_n(n)          ((n) << I350_STATUS_NUM_VFS_BIT)
#define I350_STATUS_IOV_MODE_n(n)         ((n) << I350_STATUS_IOV_MODE_BIT)
#define I350_STATUS_GIO_MES_n(n)          ((n) << I350_STATUS_GIO_MES_BIT)
#define I350_STATUS_DEV_RST_SET_n(n)      ((n) << I350_STATUS_DEV_RST_SET_BIT)
#define I350_STATUS_PF_RST_DONE_n(n)      ((n) << I350_STATUS_PF_RST_DONE_BIT)
#define I350_STATUS_MAC_CLK_GE_n(n)       ((n) << I350_STATUS_MAC_CLK_GE_BIT)

#define I350_STATUS_FD_GET(n) \
    (((n) & I350_STATUS_FD) >> I350_STATUS_FD_BIT)
#define I350_STATUS_LU_GET(n) \
    (((n) & I350_STATUS_LU) >> I350_STATUS_LU_BIT)
#define I350_STATUS_LAN_ID_GET(n) \
    (((n) & I350_STATUS_LAN_ID) >> I350_STATUS_LAN_ID_BIT)
#define I350_STATUS_TXOFF_GET(n) \
    (((n) & I350_STATUS_TXOFF) >> I350_STATUS_TXOFF_BIT)
#define I350_STATUS_SPEED_GET(n) \
    (((n) & I350_STATUS_SPEED) >> I350_STATUS_SPEED_BIT)
#define I350_STATUS_ASDV_GET(n) \
    (((n) & I350_STATUS_ASDV) >> I350_STATUS_ASDV_BIT)
#define I350_STATUS_PHYRA_GET(n) \
    (((n) & I350_STATUS_PHYRA) >> I350_STATUS_PHYRA_BIT)
#define I350_STATUS_NUM_VFS_GET(n) \
    (((n) & I350_STATUS_NUM_VFS) >> I350_STATUS_NUM_VFS_BIT)
#define I350_STATUS_IOV_MODE_GET(n) \
    (((n) & I350_STATUS_IOV_MODE) >> I350_STATUS_IOV_MODE_BIT)
#define I350_STATUS_GIO_MES_GET(n) \
    (((n) & I350_STATUS_GIO_MES) >> I350_STATUS_GIO_MES_BIT)
#define I350_STATUS_DEV_RST_SET_GET(n) \
    (((n) & I350_STATUS_DEV_RST_SET) >> I350_STATUS_DEV_RST_SET_BIT)
#define I350_STATUS_PF_RST_DONE_GET(n) \
    (((n) & I350_STATUS_PF_RST_DONE) >> I350_STATUS_PF_RST_DONE_BIT)
#define I350_STATUS_MAC_CLK_GE_GET(n) \
    (((n) & I350_STATUS_MAC_CLK_GE) >> I350_STATUS_MAC_CLK_GE_BIT)

#define I350_STATUS_FD_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_FD) | I350_STATUS_FD_n((n)))
#define I350_STATUS_LU_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_LU) | I350_STATUS_LU_n((n)))
#define I350_STATUS_LAN_ID_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_LAN_ID) | I350_STATUS_LAN_ID_n((n)))
#define I350_STATUS_TXOFF_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_TXOFF) | I350_STATUS_TXOFF_n((n)))
#define I350_STATUS_SPEED_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_SPEED) | I350_STATUS_SPEED_n((n)))
#define I350_STATUS_ASDV_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_ASDV) | I350_STATUS_ASDV_n((n)))
#define I350_STATUS_PHYRA_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_PHYRA) | I350_STATUS_PHYRA_n((n)))
#define I350_STATUS_NUM_VFS_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_NUM_VFS) | I350_STATUS_NUM_VFS_n((n)))
#define I350_STATUS_IOV_MODE_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_IOV_MODE) | I350_STATUS_IOV_MODE_n((n)))
#define I350_STATUS_GIO_MES_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_GIO_MES) | I350_STATUS_GIO_MES_n((n)))
#define I350_STATUS_DEV_RST_SET_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_DEV_RST_SET) | I350_STATUS_DEV_RST_SET_n((n)))
#define I350_STATUS_PF_RST_DONE_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_PF_RST_DONE) | I350_STATUS_PF_RST_DONE_n((n)))
#define I350_STATUS_MAC_CLK_GE_SET(r,n) \
    ((r) = ((r) & ~I350_STATUS_MAC_CLK_GE) | I350_STATUS_MAC_CLK_GE_n((n)))
