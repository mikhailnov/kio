add_executable(kioslave kioslave.cpp)
ecm_mark_nongui_executable(kioslave)

target_link_libraries(kioslave
   KF5::KIOCore
)

install(TARGETS kioslave DESTINATION ${KDE_INSTALL_LIBEXECDIR_KF5} )
