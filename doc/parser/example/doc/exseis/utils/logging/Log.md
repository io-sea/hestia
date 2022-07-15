# <a name='exseis-utils-logging-Log' /> public exseis::utils::logging::Log

The class for managing all logging activities. 



Functions in the ExSeisDat library should send messages to the user through an instance of this class, rather than directly to stdout or stderr.

The logs passed here are stored until `process_logs` is explicitly called, or the class is destroyed, at which point `process_logs` is called. 




## Private Attributes
| Name | Description | 
| ---- | ---- |
| [m_max_verbosity](#exseis-utils-logging-Log-m_max_verbosity) | The maximum verbosity level.  |
| [m_log_entries](#exseis-utils-logging-Log-m_log_entries) | A list of all the unprocessed log entries.  |
| [m_error](#exseis-utils-logging-Log-m_error) | The error status.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Log](#exseis-utils-logging-Log-Log) | Construct a new [Log][exseis-utils-logging-Log] with a given maximum verbosity.  |
| [~Log](#exseis-utils-logging-Log-~Log) | All remaining logs are processed at dectruction.  |
| [add_entry](#exseis-utils-logging-Log-add_entry) | Logs a message.  |
| [process_entries](#exseis-utils-logging-Log-process_entries) | Process the list of log entries.  |
| [has_error](#exseis-utils-logging-Log-has_error) | Check if the [Log][exseis-utils-logging-Log] object has recorded an error.  |



## Private Attributes
### <a name='exseis-utils-logging-Log-m_max_verbosity' /> private exseis::utils::logging::Log::m_max_verbosity 

The maximum verbosity level. 








[Go to Top](#exseis-utils-logging-Log)

### <a name='exseis-utils-logging-Log-m_log_entries' /> private exseis::utils::logging::Log::m_log_entries 

A list of all the unprocessed log entries. 








[Go to Top](#exseis-utils-logging-Log)

### <a name='exseis-utils-logging-Log-m_error' /> private exseis::utils::logging::Log::m_error  = false

The error status. 








[Go to Top](#exseis-utils-logging-Log)

## Public Functions
### <a name='exseis-utils-logging-Log-Log' /> public  exseis::utils::logging::Log::Log (Verbosity max_verbosity=exseis::utils::Verbosity::none)

Construct a new [Log][exseis-utils-logging-Log] with a given maximum verbosity. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Verbosity][exseis-utils-logging-Verbosity] | max_verbosity | The maximum verbosity level of stored logs.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-logging-Log)

### <a name='exseis-utils-logging-Log-~Log' /> public  exseis::utils::logging::Log::~Log ()

All remaining logs are processed at dectruction. 








#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-logging-Log)

### <a name='exseis-utils-logging-Log-add_entry' /> public void exseis::utils::logging::Log::add_entry (Log_entry log_entry)

Logs a message. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Log_entry][exseis-utils-logging-Log_entry] | log_entry | An [`Log_entry`][exseis-utils-logging-Log_entry] to log.  |












[Go to Top](#exseis-utils-logging-Log)

### <a name='exseis-utils-logging-Log-process_entries' /> public void exseis::utils::logging::Log::process_entries ()

Process the list of log entries. 








[Go to Top](#exseis-utils-logging-Log)

### <a name='exseis-utils-logging-Log-has_error' /> public bool exseis::utils::logging::Log::has_error () const

Check if the [Log][exseis-utils-logging-Log] object has recorded an error. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool |  |







This is set to `true` the first time a log entry is encountered with the `Error` status, and never set back to `false`.


> **retval true:** An error has been encountered. 



> **retval false:** An error has not been encountered. 




#### Qualifiers: 
* const


[Go to Top](#exseis-utils-logging-Log)

[exseis-utils-logging-Log]:./Log.md
[exseis-utils-logging-Log_entry]:./Log_entry.md#exseis-utils-logging-Log_entry
[exseis-utils-logging-Verbosity]:./index.md#exseis-utils-logging-Verbosity
