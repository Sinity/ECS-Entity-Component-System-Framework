#pragma once

namespace EECS {
class ComponentContainerID {
   public:
    template <typename T>
    static size_t get() {
        static size_t id = counter++;
        return id;
    }

   private:
    static size_t counter;
};
}
