class HestiaConfigurationError(Exception):
    pass

__all__ = [name for name in dir() if name.endswith("Error")]