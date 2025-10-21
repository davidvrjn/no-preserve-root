 #pragma once
 #include <any> // Using std::any or a custom struct for flexible state storage

 /**
  * @class Memento
  * @brief The Memento object for the Memento design pattern.
  * 
  * This is a "dumb" data container. Its only job is to store a snapshot of the
  * Nursery's state. It provides a getter for the state but should have no knowledge
  * of the Nursery class itself to maintain encapsulation.
  */
 class Memento {
 private:
	 // The state can be a complex struct or a map containing all necessary data.
	 // We'll use a placeholder for now.
	 struct NurseryState {
		 int day;
		 // ... data for inventory, staff, etc.
	 };
	 NurseryState state;

 public:
	 Memento(const NurseryState& state);
	 ~Memento();

	 NurseryState getState() const;
 };
