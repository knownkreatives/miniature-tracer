class ExampleClass:
    def __init__(self, value):
        self.value = value

    def increment(self, amount):
        self.value += amount
        return self.value

    def get_value(self):
        return self.value
    
if __name__ == "__main__":
    example = ExampleClass(10)
    print("Initial value:", example.get_value())
    print("After incrementing by 5:", example.increment(5))
    print("After incrementing by 10:", example.increment(10))