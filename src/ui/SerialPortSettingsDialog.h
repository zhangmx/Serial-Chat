#ifndef SERIAL_PORT_SETTINGS_DIALOG_H
#define SERIAL_PORT_SETTINGS_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "SerialPortInfo.h"

/**
 * @brief Dialog for configuring serial port settings
 */
class SerialPortSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SerialPortSettingsDialog(QWidget* parent = nullptr);
    explicit SerialPortSettingsDialog(const SerialPortInfo& info, QWidget* parent = nullptr);
    ~SerialPortSettingsDialog() override;
    
    // Get the configured settings
    SerialPortInfo portInfo() const;
    
    // Set initial values
    void setPortInfo(const SerialPortInfo& info);
    
    // Refresh available ports
    void refreshPorts();

private slots:
    void onRefreshClicked();
    void onOkClicked();
    void onCancelClicked();

private:
    SerialPortInfo m_info;
    bool m_editMode;
    
    // UI Components
    QVBoxLayout* m_mainLayout;
    QFormLayout* m_formLayout;
    
    QComboBox* m_portCombo;
    QPushButton* m_refreshButton;
    QComboBox* m_baudRateCombo;
    QComboBox* m_dataBitsCombo;
    QComboBox* m_stopBitsCombo;
    QComboBox* m_parityCombo;
    QComboBox* m_flowControlCombo;
    
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    
    void setupUi();
    void populateCombos();
    void loadSettings();
    void saveSettings();
};

#endif // SERIAL_PORT_SETTINGS_DIALOG_H
