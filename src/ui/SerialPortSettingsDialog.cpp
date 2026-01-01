#include "SerialPortSettingsDialog.h"
#include <QSerialPortInfo>

SerialPortSettingsDialog::SerialPortSettingsDialog(QWidget* parent)
    : QDialog(parent)
    , m_editMode(false)
{
    setupUi();
    populateCombos();
    refreshPorts();
}

SerialPortSettingsDialog::SerialPortSettingsDialog(const SerialPortInfo& info, QWidget* parent)
    : QDialog(parent)
    , m_info(info)
    , m_editMode(true)
{
    setupUi();
    populateCombos();
    refreshPorts();
    loadSettings();
}

SerialPortSettingsDialog::~SerialPortSettingsDialog()
{
}

SerialPortInfo SerialPortSettingsDialog::portInfo() const
{
    return m_info;
}

void SerialPortSettingsDialog::setPortInfo(const SerialPortInfo& info)
{
    m_info = info;
    m_editMode = true;
    loadSettings();
}

void SerialPortSettingsDialog::refreshPorts()
{
    QString currentPort = m_portCombo->currentText();
    m_portCombo->clear();
    
    const auto ports = QSerialPortInfo::availablePorts();
    for (const auto& port : ports) {
        QString displayText = port.portName();
        if (!port.description().isEmpty()) {
            displayText += " - " + port.description();
        }
        m_portCombo->addItem(displayText, port.portName());
    }
    
    // Restore selection if possible
    int index = m_portCombo->findData(currentPort);
    if (index >= 0) {
        m_portCombo->setCurrentIndex(index);
    } else if (!m_info.portName().isEmpty()) {
        index = m_portCombo->findData(m_info.portName());
        if (index >= 0) {
            m_portCombo->setCurrentIndex(index);
        }
    }
}

void SerialPortSettingsDialog::onRefreshClicked()
{
    refreshPorts();
}

void SerialPortSettingsDialog::onOkClicked()
{
    saveSettings();
    accept();
}

void SerialPortSettingsDialog::onCancelClicked()
{
    reject();
}

void SerialPortSettingsDialog::setupUi()
{
    setWindowTitle(m_editMode ? tr("Edit Port Settings") : tr("Add Serial Port"));
    setMinimumWidth(350);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    m_formLayout = new QFormLayout();
    m_formLayout->setSpacing(10);
    
    // Port selection with refresh button
    QWidget* portWidget = new QWidget(this);
    QHBoxLayout* portLayout = new QHBoxLayout(portWidget);
    portLayout->setContentsMargins(0, 0, 0, 0);
    portLayout->setSpacing(5);
    
    m_portCombo = new QComboBox(portWidget);
    m_portCombo->setMinimumWidth(180);
    
    m_refreshButton = new QPushButton(tr("↻"), portWidget);
    m_refreshButton->setFixedWidth(30);
    m_refreshButton->setToolTip(tr("Refresh port list"));
    connect(m_refreshButton, &QPushButton::clicked, this, &SerialPortSettingsDialog::onRefreshClicked);
    
    portLayout->addWidget(m_portCombo, 1);
    portLayout->addWidget(m_refreshButton);
    
    m_baudRateCombo = new QComboBox(this);
    m_dataBitsCombo = new QComboBox(this);
    m_stopBitsCombo = new QComboBox(this);
    m_parityCombo = new QComboBox(this);
    m_flowControlCombo = new QComboBox(this);
    
    m_formLayout->addRow(tr("Port:"), portWidget);
    m_formLayout->addRow(tr("Baud Rate:"), m_baudRateCombo);
    m_formLayout->addRow(tr("Data Bits:"), m_dataBitsCombo);
    m_formLayout->addRow(tr("Stop Bits:"), m_stopBitsCombo);
    m_formLayout->addRow(tr("Parity:"), m_parityCombo);
    m_formLayout->addRow(tr("Flow Control:"), m_flowControlCombo);
    
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(10);
    
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_cancelButton->setStyleSheet("QPushButton { padding: 8px 20px; }");
    connect(m_cancelButton, &QPushButton::clicked, this, &SerialPortSettingsDialog::onCancelClicked);
    
    m_okButton = new QPushButton(tr("OK"), this);
    m_okButton->setDefault(true);
    m_okButton->setStyleSheet("QPushButton { padding: 8px 30px; background-color: #07C160; color: white; border: none; border-radius: 5px; } QPushButton:hover { background-color: #06AD56; }");
    connect(m_okButton, &QPushButton::clicked, this, &SerialPortSettingsDialog::onOkClicked);
    
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addWidget(m_okButton);
    
    m_mainLayout->addLayout(m_formLayout);
    m_mainLayout->addStretch();
    m_mainLayout->addLayout(m_buttonLayout);
}

void SerialPortSettingsDialog::populateCombos()
{
    // Baud rates
    QList<qint32> baudRates = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
    for (qint32 rate : baudRates) {
        m_baudRateCombo->addItem(QString::number(rate), rate);
    }
    m_baudRateCombo->setCurrentText("115200");
    
    // Data bits
    m_dataBitsCombo->addItem("5", QSerialPort::Data5);
    m_dataBitsCombo->addItem("6", QSerialPort::Data6);
    m_dataBitsCombo->addItem("7", QSerialPort::Data7);
    m_dataBitsCombo->addItem("8", QSerialPort::Data8);
    m_dataBitsCombo->setCurrentText("8");
    
    // Stop bits
    m_stopBitsCombo->addItem("1", QSerialPort::OneStop);
    m_stopBitsCombo->addItem("1.5", QSerialPort::OneAndHalfStop);
    m_stopBitsCombo->addItem("2", QSerialPort::TwoStop);
    m_stopBitsCombo->setCurrentIndex(0);
    
    // Parity
    m_parityCombo->addItem(tr("None"), QSerialPort::NoParity);
    m_parityCombo->addItem(tr("Even"), QSerialPort::EvenParity);
    m_parityCombo->addItem(tr("Odd"), QSerialPort::OddParity);
    m_parityCombo->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_parityCombo->addItem(tr("Space"), QSerialPort::SpaceParity);
    m_parityCombo->setCurrentIndex(0);
    
    // Flow control
    m_flowControlCombo->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_flowControlCombo->addItem(tr("Hardware (RTS/CTS)"), QSerialPort::HardwareControl);
    m_flowControlCombo->addItem(tr("Software (XON/XOFF)"), QSerialPort::SoftwareControl);
    m_flowControlCombo->setCurrentIndex(0);
}

void SerialPortSettingsDialog::loadSettings()
{
    // Port
    int portIndex = m_portCombo->findData(m_info.portName());
    if (portIndex >= 0) {
        m_portCombo->setCurrentIndex(portIndex);
    }
    
    // Baud rate
    int baudIndex = m_baudRateCombo->findData(m_info.baudRate());
    if (baudIndex >= 0) {
        m_baudRateCombo->setCurrentIndex(baudIndex);
    }
    
    // Data bits
    int dataIndex = m_dataBitsCombo->findData(static_cast<int>(m_info.dataBits()));
    if (dataIndex >= 0) {
        m_dataBitsCombo->setCurrentIndex(dataIndex);
    }
    
    // Stop bits
    int stopIndex = m_stopBitsCombo->findData(static_cast<int>(m_info.stopBits()));
    if (stopIndex >= 0) {
        m_stopBitsCombo->setCurrentIndex(stopIndex);
    }
    
    // Parity
    int parityIndex = m_parityCombo->findData(static_cast<int>(m_info.parity()));
    if (parityIndex >= 0) {
        m_parityCombo->setCurrentIndex(parityIndex);
    }
    
    // Flow control
    int flowIndex = m_flowControlCombo->findData(static_cast<int>(m_info.flowControl()));
    if (flowIndex >= 0) {
        m_flowControlCombo->setCurrentIndex(flowIndex);
    }
}

void SerialPortSettingsDialog::saveSettings()
{
    m_info.setPortName(m_portCombo->currentData().toString());
    m_info.setBaudRate(m_baudRateCombo->currentData().toInt());
    m_info.setDataBits(static_cast<QSerialPort::DataBits>(m_dataBitsCombo->currentData().toInt()));
    m_info.setStopBits(static_cast<QSerialPort::StopBits>(m_stopBitsCombo->currentData().toInt()));
    m_info.setParity(static_cast<QSerialPort::Parity>(m_parityCombo->currentData().toInt()));
    m_info.setFlowControl(static_cast<QSerialPort::FlowControl>(m_flowControlCombo->currentData().toInt()));
}
