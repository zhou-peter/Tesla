﻿//------------------------------------------------------------------------------
// <auto-generated>
//     Этот код создан программой.
//     Исполняемая версия:4.0.30319.42000
//
//     Изменения в этом файле могут привести к неправильной работе и будут потеряны в случае
//     повторной генерации кода.
// </auto-generated>
//------------------------------------------------------------------------------

namespace TeslaComClient.TeslaCommunication {
    using System.Runtime.Serialization;
    using System;
    
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="HardwareState", Namespace="http://schemas.datacontract.org/2004/07/TeslaCommunication")]
    [System.SerializableAttribute()]
    public partial class HardwareState : object, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        private int currentPeriodField;
        
        private TeslaComClient.TeslaCommunication.HardwareState.SearchState currentStateField;
        
        private bool enabledF1Field;
        
        private bool enabledF2Field;
        
        private bool enabledF3Field;
        
        private bool enabledF4Field;
        
        private bool enabledF5Field;
        
        private bool enabledF6Field;
        
        private bool ledLightField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int currentPeriod {
            get {
                return this.currentPeriodField;
            }
            set {
                if ((this.currentPeriodField.Equals(value) != true)) {
                    this.currentPeriodField = value;
                    this.RaisePropertyChanged("currentPeriod");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public TeslaComClient.TeslaCommunication.HardwareState.SearchState currentState {
            get {
                return this.currentStateField;
            }
            set {
                if ((this.currentStateField.Equals(value) != true)) {
                    this.currentStateField = value;
                    this.RaisePropertyChanged("currentState");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public bool enabledF1 {
            get {
                return this.enabledF1Field;
            }
            set {
                if ((this.enabledF1Field.Equals(value) != true)) {
                    this.enabledF1Field = value;
                    this.RaisePropertyChanged("enabledF1");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public bool enabledF2 {
            get {
                return this.enabledF2Field;
            }
            set {
                if ((this.enabledF2Field.Equals(value) != true)) {
                    this.enabledF2Field = value;
                    this.RaisePropertyChanged("enabledF2");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public bool enabledF3 {
            get {
                return this.enabledF3Field;
            }
            set {
                if ((this.enabledF3Field.Equals(value) != true)) {
                    this.enabledF3Field = value;
                    this.RaisePropertyChanged("enabledF3");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public bool enabledF4 {
            get {
                return this.enabledF4Field;
            }
            set {
                if ((this.enabledF4Field.Equals(value) != true)) {
                    this.enabledF4Field = value;
                    this.RaisePropertyChanged("enabledF4");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public bool enabledF5 {
            get {
                return this.enabledF5Field;
            }
            set {
                if ((this.enabledF5Field.Equals(value) != true)) {
                    this.enabledF5Field = value;
                    this.RaisePropertyChanged("enabledF5");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public bool enabledF6 {
            get {
                return this.enabledF6Field;
            }
            set {
                if ((this.enabledF6Field.Equals(value) != true)) {
                    this.enabledF6Field = value;
                    this.RaisePropertyChanged("enabledF6");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public bool ledLight {
            get {
                return this.ledLightField;
            }
            set {
                if ((this.ledLightField.Equals(value) != true)) {
                    this.ledLightField = value;
                    this.RaisePropertyChanged("ledLight");
                }
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        protected void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
        
        [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
        [System.Runtime.Serialization.DataContractAttribute(Name="HardwareState.SearchState", Namespace="http://schemas.datacontract.org/2004/07/TeslaCommunication")]
        public enum SearchState : int {
            
            [System.Runtime.Serialization.EnumMemberAttribute()]
            Idle = 0,
            
            [System.Runtime.Serialization.EnumMemberAttribute()]
            Searching = 1,
            
            [System.Runtime.Serialization.EnumMemberAttribute()]
            Generating = 2,
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="TimersConfiguration", Namespace="http://schemas.datacontract.org/2004/07/TeslaCommunication")]
    [System.SerializableAttribute()]
    public partial class TimersConfiguration : object, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        private int dutyBunchField;
        
        private int offGapField;
        
        private int onGapField;
        
        private int periodBunchField;
        
        private int periodCarrierField;
        
        private int periodGapField;
        
        private int startGapField;
        
        private int startHighField;
        
        private int startLowField;
        
        private int stopGapField;
        
        private int stopHighField;
        
        private int stopLowField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int dutyBunch {
            get {
                return this.dutyBunchField;
            }
            set {
                if ((this.dutyBunchField.Equals(value) != true)) {
                    this.dutyBunchField = value;
                    this.RaisePropertyChanged("dutyBunch");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int offGap {
            get {
                return this.offGapField;
            }
            set {
                if ((this.offGapField.Equals(value) != true)) {
                    this.offGapField = value;
                    this.RaisePropertyChanged("offGap");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int onGap {
            get {
                return this.onGapField;
            }
            set {
                if ((this.onGapField.Equals(value) != true)) {
                    this.onGapField = value;
                    this.RaisePropertyChanged("onGap");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int periodBunch {
            get {
                return this.periodBunchField;
            }
            set {
                if ((this.periodBunchField.Equals(value) != true)) {
                    this.periodBunchField = value;
                    this.RaisePropertyChanged("periodBunch");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int periodCarrier {
            get {
                return this.periodCarrierField;
            }
            set {
                if ((this.periodCarrierField.Equals(value) != true)) {
                    this.periodCarrierField = value;
                    this.RaisePropertyChanged("periodCarrier");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int periodGap {
            get {
                return this.periodGapField;
            }
            set {
                if ((this.periodGapField.Equals(value) != true)) {
                    this.periodGapField = value;
                    this.RaisePropertyChanged("periodGap");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int startGap {
            get {
                return this.startGapField;
            }
            set {
                if ((this.startGapField.Equals(value) != true)) {
                    this.startGapField = value;
                    this.RaisePropertyChanged("startGap");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int startHigh {
            get {
                return this.startHighField;
            }
            set {
                if ((this.startHighField.Equals(value) != true)) {
                    this.startHighField = value;
                    this.RaisePropertyChanged("startHigh");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int startLow {
            get {
                return this.startLowField;
            }
            set {
                if ((this.startLowField.Equals(value) != true)) {
                    this.startLowField = value;
                    this.RaisePropertyChanged("startLow");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int stopGap {
            get {
                return this.stopGapField;
            }
            set {
                if ((this.stopGapField.Equals(value) != true)) {
                    this.stopGapField = value;
                    this.RaisePropertyChanged("stopGap");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int stopHigh {
            get {
                return this.stopHighField;
            }
            set {
                if ((this.stopHighField.Equals(value) != true)) {
                    this.stopHighField = value;
                    this.RaisePropertyChanged("stopHigh");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public int stopLow {
            get {
                return this.stopLowField;
            }
            set {
                if ((this.stopLowField.Equals(value) != true)) {
                    this.stopLowField = value;
                    this.RaisePropertyChanged("stopLow");
                }
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        protected void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ServiceModel.ServiceContractAttribute(Namespace="http://STM32TeslaCommunication", ConfigurationName="TeslaCommunication.ICommunicationProtocol")]
    public interface ICommunicationProtocol {
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/Connect", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/ConnectResponse")]
        bool Connect(string comPortName);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/Connect", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/ConnectResponse")]
        System.Threading.Tasks.Task<bool> ConnectAsync(string comPortName);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/IsConnected", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/IsConnectedResponse")]
        bool IsConnected();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/IsConnected", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/IsConnectedResponse")]
        System.Threading.Tasks.Task<bool> IsConnectedAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/Disconnect", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/DisconnectResponse")]
        void Disconnect();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/Disconnect", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/DisconnectResponse")]
        System.Threading.Tasks.Task DisconnectAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/ClearQueues", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/ClearQueuesResponse")]
        void ClearQueues();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/ClearQueues", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/ClearQueuesResponse")]
        System.Threading.Tasks.Task ClearQueuesAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/setEnabled", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/setEnabledResponse")]
        void setEnabled(byte num, bool enabled);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/setEnabled", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/setEnabledResponse")]
        System.Threading.Tasks.Task setEnabledAsync(byte num, bool enabled);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/getHardwareState", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/getHardwareStateResponse")]
        TeslaComClient.TeslaCommunication.HardwareState getHardwareState();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/getHardwareState", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/getHardwareStateResponse")]
        System.Threading.Tasks.Task<TeslaComClient.TeslaCommunication.HardwareState> getHardwareStateAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/setTimersConfiguration", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/setTimersConfigurationRespo" +
            "nse")]
        void setTimersConfiguration(TeslaComClient.TeslaCommunication.TimersConfiguration timersConfiguration);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/setTimersConfiguration", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/setTimersConfigurationRespo" +
            "nse")]
        System.Threading.Tasks.Task setTimersConfigurationAsync(TeslaComClient.TeslaCommunication.TimersConfiguration timersConfiguration);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/searchStart", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/searchStartResponse")]
        void searchStart(int periodStart, int periodStop, int delay);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/searchStart", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/searchStartResponse")]
        System.Threading.Tasks.Task searchStartAsync(int periodStart, int periodStop, int delay);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/searchStop", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/searchStopResponse")]
        void searchStop();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/searchStop", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/searchStopResponse")]
        System.Threading.Tasks.Task searchStopAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/searchGeneratePWM", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/searchGeneratePWMResponse")]
        void searchGeneratePWM(int period);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://STM32TeslaCommunication/ICommunicationProtocol/searchGeneratePWM", ReplyAction="http://STM32TeslaCommunication/ICommunicationProtocol/searchGeneratePWMResponse")]
        System.Threading.Tasks.Task searchGeneratePWMAsync(int period);
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public interface ICommunicationProtocolChannel : TeslaComClient.TeslaCommunication.ICommunicationProtocol, System.ServiceModel.IClientChannel {
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public partial class CommunicationProtocolClient : System.ServiceModel.ClientBase<TeslaComClient.TeslaCommunication.ICommunicationProtocol>, TeslaComClient.TeslaCommunication.ICommunicationProtocol {
        
        public CommunicationProtocolClient() {
        }
        
        public CommunicationProtocolClient(string endpointConfigurationName) : 
                base(endpointConfigurationName) {
        }
        
        public CommunicationProtocolClient(string endpointConfigurationName, string remoteAddress) : 
                base(endpointConfigurationName, remoteAddress) {
        }
        
        public CommunicationProtocolClient(string endpointConfigurationName, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(endpointConfigurationName, remoteAddress) {
        }
        
        public CommunicationProtocolClient(System.ServiceModel.Channels.Binding binding, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(binding, remoteAddress) {
        }
        
        public bool Connect(string comPortName) {
            return base.Channel.Connect(comPortName);
        }
        
        public System.Threading.Tasks.Task<bool> ConnectAsync(string comPortName) {
            return base.Channel.ConnectAsync(comPortName);
        }
        
        public bool IsConnected() {
            return base.Channel.IsConnected();
        }
        
        public System.Threading.Tasks.Task<bool> IsConnectedAsync() {
            return base.Channel.IsConnectedAsync();
        }
        
        public void Disconnect() {
            base.Channel.Disconnect();
        }
        
        public System.Threading.Tasks.Task DisconnectAsync() {
            return base.Channel.DisconnectAsync();
        }
        
        public void ClearQueues() {
            base.Channel.ClearQueues();
        }
        
        public System.Threading.Tasks.Task ClearQueuesAsync() {
            return base.Channel.ClearQueuesAsync();
        }
        
        public void setEnabled(byte num, bool enabled) {
            base.Channel.setEnabled(num, enabled);
        }
        
        public System.Threading.Tasks.Task setEnabledAsync(byte num, bool enabled) {
            return base.Channel.setEnabledAsync(num, enabled);
        }
        
        public TeslaComClient.TeslaCommunication.HardwareState getHardwareState() {
            return base.Channel.getHardwareState();
        }
        
        public System.Threading.Tasks.Task<TeslaComClient.TeslaCommunication.HardwareState> getHardwareStateAsync() {
            return base.Channel.getHardwareStateAsync();
        }
        
        public void setTimersConfiguration(TeslaComClient.TeslaCommunication.TimersConfiguration timersConfiguration) {
            base.Channel.setTimersConfiguration(timersConfiguration);
        }
        
        public System.Threading.Tasks.Task setTimersConfigurationAsync(TeslaComClient.TeslaCommunication.TimersConfiguration timersConfiguration) {
            return base.Channel.setTimersConfigurationAsync(timersConfiguration);
        }
        
        public void searchStart(int periodStart, int periodStop, int delay) {
            base.Channel.searchStart(periodStart, periodStop, delay);
        }
        
        public System.Threading.Tasks.Task searchStartAsync(int periodStart, int periodStop, int delay) {
            return base.Channel.searchStartAsync(periodStart, periodStop, delay);
        }
        
        public void searchStop() {
            base.Channel.searchStop();
        }
        
        public System.Threading.Tasks.Task searchStopAsync() {
            return base.Channel.searchStopAsync();
        }
        
        public void searchGeneratePWM(int period) {
            base.Channel.searchGeneratePWM(period);
        }
        
        public System.Threading.Tasks.Task searchGeneratePWMAsync(int period) {
            return base.Channel.searchGeneratePWMAsync(period);
        }
    }
}
