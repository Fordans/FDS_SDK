/*
        Copyright(C) 2025 Fordans
                        This source follows the GPL licence
                        See https://www.gnu.org/licenses/gpl-3.0.html for details
*/

#pragma once

#ifdef _DEBUG
#define FDS_CONDITIONAL_COMPILE(x) do{x}while(false)
#else
#define FDS_CONDITIONAL_COMPILE(x)
#endif