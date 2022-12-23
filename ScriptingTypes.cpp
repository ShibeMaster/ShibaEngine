#include "ScriptingTypes.h"
#include "Time.h"
#include <iostream>

MonoMethod* Class::GetMethod(const std::string& name, int parameterCount) {
	return mono_class_get_method_from_name(monoClass, name.c_str(), parameterCount);
}
ClassInstance::ClassInstance(Class klass, bool core) {
	this->classData = klass;
	if (!core) {
		startMethod = classData.GetMethod("Start", 0);
		hasStart = startMethod != nullptr;
		updateMethod = classData.GetMethod("Update", 0);
		hasUpdate = updateMethod != nullptr;
	}
}
void ClassInstance::InvokeMethod(MonoMethod* method, int parameterCount, void** params) {
	MonoObject* exception = nullptr;
	mono_runtime_invoke(method, instance, params, &exception);
}
bool Behaviour::ShouldUpdate() {
	return !hasInterval || Time::currentTime - lastUpdateTime > interval;
}	