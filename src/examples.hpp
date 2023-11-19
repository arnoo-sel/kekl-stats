#pragma once

#include "auth.hpp"

// This will print a list of all KEKL mappers with how many KEKL maps they built
QCoro::Task<> keklMapCountPerMapper(qttm::Authorization& auth);

// This will print all the personal bests set by the specified  players on all KEKL maps
QCoro::Task<> keklMapPlayedByPlayers(qttm::Authorization& auth, QStringList playerIds);
