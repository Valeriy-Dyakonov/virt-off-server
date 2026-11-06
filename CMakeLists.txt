cmake_minimum_required(VERSION 3.20)
project(virt-off-server VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# ---- Fetch asio (standalone, header-only) ----
include(FetchContent)
FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG        asio-1-30-2
)
FetchContent_MakeAvailable(asio)

# ---- Sources ----
# Legacy V1 API plus the transport and logging shared with V2.
set(V1_SOURCES
    src/app/CliParser.cpp
    src/app/Server.cpp
    src/app/ServerBuilder.cpp
    src/app/ServerFactory.cpp
    src/clients/ClientRepository.cpp
    src/core/ConsoleLogger.cpp
    src/handlers/ConnectHandler.cpp
    src/handlers/RelayHandler.cpp
    src/net/PacketRouter.cpp
    src/net/UdpTransport.cpp
)

# V2 API: one folder per feature module (see README, Architecture).
set(V2_SOURCES
    src/v2/app/Server.cpp
    src/v2/app/ServerBuilder.cpp
    src/v2/clients/UserRepository.cpp
    src/v2/core/TimestampIdGenerator.cpp
    src/v2/game/combat/AttackGate.cpp
    src/v2/game/combat/AttackHandler.cpp
    src/v2/game/combat/AttackTargetReader.cpp
    src/v2/game/combat/CombatModule.cpp
    src/v2/game/players/PlayerModule.cpp
    src/v2/game/players/PlayerState.cpp
    src/v2/game/players/PositionReader.cpp
    src/v2/game/players/PositionTracker.cpp
    src/v2/game/props/PropModule.cpp
    src/v2/game/props/codec/ManifestAckEncoder.cpp
    src/v2/game/props/codec/ManifestReader.cpp
    src/v2/game/props/codec/PropIdReader.cpp
    src/v2/game/props/codec/PropStateReader.cpp
    src/v2/game/props/codec/ReleaseEventEncoder.cpp
    src/v2/game/props/events/GrantAnnouncer.cpp
    src/v2/game/props/events/PropReleaser.cpp
    src/v2/game/props/handlers/ManifestHandler.cpp
    src/v2/game/props/handlers/PickupHandler.cpp
    src/v2/game/props/handlers/PropStateHandler.cpp
    src/v2/game/props/handlers/PushHandler.cpp
    src/v2/game/props/handlers/ReleaseHandler.cpp
    src/v2/game/props/registry/PropReach.cpp
    src/v2/game/props/registry/PropRepository.cpp
    src/v2/game/props/registry/SimulationSweeper.cpp
    src/v2/game/session/ActivityTracker.cpp
    src/v2/game/session/ConnectHandler.cpp
    src/v2/game/session/DisconnectHandler.cpp
    src/v2/game/session/IdleSessionSweeper.cpp
    src/v2/game/session/SessionModule.cpp
    src/v2/game/session/SessionTerminator.cpp
    src/v2/names/NameHandler.cpp
    src/v2/names/NameModule.cpp
    src/v2/names/codec/PlayerNameEncoder.cpp
    src/v2/names/codec/PlayerNameReader.cpp
    src/v2/names/codec/PlayerNameSanitizer.cpp
    src/v2/names/events/NameBroadcaster.cpp
    src/v2/names/events/NameRoster.cpp
    src/v2/net/broadcast/Broadcaster.cpp
    src/v2/net/wire/SenderStamp.cpp
    src/v2/net/wire/WireReader.cpp
    src/v2/net/wire/WireWriter.cpp
    src/v2/relay/RelayHandler.cpp
    src/v2/relay/RelayModule.cpp
)

# ---- Library ----
add_library(virt-off-server-core STATIC
    ${V1_SOURCES}
    ${V2_SOURCES}
)

target_include_directories(virt-off-server-core PUBLIC src)
target_include_directories(virt-off-server-core PUBLIC ${asio_SOURCE_DIR}/asio/include)
if(WIN32)
    # asio на Windows (MinGW из CLion) работает через Winsock; целевая версия — Windows 10.
    target_link_libraries(virt-off-server-core PUBLIC ws2_32 mswsock)
    target_compile_definitions(virt-off-server-core PUBLIC _WIN32_WINNT=0x0A00)
endif()

# ---- Executable ----
add_executable(virt-off-server src/main.cpp)
target_link_libraries(virt-off-server PRIVATE virt-off-server-core Threads::Threads)

# ---- Tests ----
enable_testing()
add_test(
    NAME smoke_server
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/smoke_server_test.sh $<TARGET_FILE:virt-off-server>
)

# ---- Unit tests: parsers and prop ownership rules ----
add_executable(unit_tests tests/unit/unit_tests.cpp)
target_link_libraries(unit_tests PRIVATE virt-off-server-core Threads::Threads)
add_test(NAME unit COMMAND unit_tests)

# ---- Integration test client (out of the server project code) ----
add_executable(test_client tests/integration/test_client.cpp)
target_include_directories(test_client PRIVATE ${asio_SOURCE_DIR}/asio/include)
target_link_libraries(test_client PRIVATE Threads::Threads)
if(WIN32)
    target_link_libraries(test_client PRIVATE ws2_32 mswsock)
    target_compile_definitions(test_client PRIVATE _WIN32_WINNT=0x0A00)
endif()

# Multiplayer integration test: real v2 server + listener/sender clients.
# Registered in CTest but NOT run automatically on every build (slower,
# timing/network sensitive) — run it explicitly with: ctest -R integration_server
add_test(
    NAME integration_server
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/integration_test.sh
            $<TARGET_FILE:virt-off-server> $<TARGET_FILE:test_client>
)

# Re-run the smoke tests after every build. Skipped for cross-arch builds
# (e.g. x86_64 on Apple Silicon) where the produced binary cannot run locally.
if(NOT CMAKE_OSX_ARCHITECTURES)
    add_custom_target(run_smoke_tests ALL
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R "smoke_server|unit"
        DEPENDS virt-off-server test_client unit_tests
        COMMENT "Running smoke and unit tests"
        VERBATIM
    )
endif()
