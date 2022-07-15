namespace regression {
    
    ///
    /// @brief typedef for reference
    ///
    typedef int referenced_type;
    
    ///
    /// @brief mixing code and references
    ///
    /// In case a small code output (larger codeblocks don't work with references
    /// in Markdown anyway - different problem!) contains a reference, the code 
    /// identifiers \' have to be inside the brackets \[ \] for the reference. 
    /// Since these are read before the reference is evaluated, they would be 
    /// outside automatically:
    ///
    /// Code: `regression::referenced_type`
    void function_with_reference_in_code();
}