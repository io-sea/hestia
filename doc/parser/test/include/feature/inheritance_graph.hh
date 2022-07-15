namespace feature {
    /// 
    /// @brief this is the base class, the other one will inherit
    ///
    class Base{
        ///
        /// @brief virtual function to be overridden by  @ref feature::Derived::override_function 
        ///
        virtual void override_function (){}
    };
    
    /// 
    /// @brief this is the derived class, that inherits the base class
    ///
    class Derived : Base {
        ///
        /// @brief function to override @ref feature::Base::override_function 
        ///
        void override_function () override {}
    };
}