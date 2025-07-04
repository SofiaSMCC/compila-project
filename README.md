# Informe del Proyecto de Compiladores

**Extensiones Implementadas**

**Integrantes:** Sofia, Abigail, Jeffry  
**Fecha:** \today  

---

## Introducción

## Extensiones Implementadas

### Clases Elaboradas

#### Environment

**Archivo:** `environment.h`

**Propósito:** Gestión de ámbitos anidados y tablas de símbolos con soporte extendido para arrays multidimensionales y tipos unificados.

```cpp
#ifndef COMPILA_PROJECT_ENVIRONMENT_HH
#define COMPILA_PROJECT_ENVIRONMENT_HH

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
