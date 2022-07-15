namespace structure {

    ///
    /// @brief normal enum
    ///
    enum normal_enum{
        /// Enum value 1
        one,
        /// Enum value 2
        two
    };

    ///
    /// @brief class enum
    ///
    enum class class_enum{
        /// Enum value 1
        one,
        /// Enum value 2
        two
    };

    ///
    /// @brief class enum with inheritance
    ///
    enum class class_enum_inheritance : int {
        /// Enum value 1
        one,
        /// Enum value 2
        two
    };

    ///
    /// @brief unnamed enum
    ///
    enum {
        /// Enum value 1
        one,
        /// Enum value 2
        two
    };
}
