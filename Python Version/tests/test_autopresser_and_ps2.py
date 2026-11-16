from tests._qt_stub import install_qt_stubs_if_needed
install_qt_stubs_if_needed()

import unittest
import tempfile
from pathlib import Path

from app.config import AppConfig
from app.domain.autopresser_engine import (
    AUTOPRESSER_KEY_METHODS,
    AUTOPRESSER_MOUSE_MODES,
    BUILTIN_36_KEYS,
    AutoKeyPresserEngine,
    AutoPresserKeyItem,
    AutoPresserMousePoint,
)
from app.domain.win32_input import (
    diagnose_ps2_port_driver,
    format_ps2_diagnostic_report,
)
from app.i18n import SUPPORTED_LANGUAGES, set_language, t
from app.domain.autopresser_engine import get_input_method_options


class TestAutoPresserAndPs2(unittest.TestCase):
    def test_builtin_36_keys_and_20_unified_methods_and_8_mouse_modes(self) -> None:
        self.assertEqual(len(BUILTIN_36_KEYS), 36)
        self.assertEqual(len(AUTOPRESSER_KEY_METHODS), 22)
        self.assertEqual(len(get_input_method_options()), 22)
        self.assertEqual(len(AUTOPRESSER_MOUSE_MODES), 8)
        cfg = AppConfig()
        self.assertEqual(cfg.hotkey_autopresser_stop_all, "F7")

    def test_multi_tab_lifecycle_and_sweep_and_serialization(self) -> None:
        from app.paths import get_appdata_dir
        ws_def = get_appdata_dir() / "autopresser_workspace.json"
        if ws_def.exists():
            ws_def.unlink()

        eng = AutoKeyPresserEngine()
        self.assertGreaterEqual(len(eng.tabs), 1)
        t1 = eng.tabs[0]
        self.assertEqual(len(t1.keys), 36)
        self.assertEqual(t1.stop_hotkey, "F6")

        # Add custom key and mouse point
        ck = AutoPresserKeyItem(key="Space", enabled=True, interval_ms=250, is_custom=True)
        t1.keys.append(ck)
        self.assertEqual(len(t1.keys), 37)

        mp = AutoPresserMousePoint(x=500, y=300, button="right", interval_ms=400, enabled=True)
        t1.mouse_points.append(mp)
        self.assertEqual(len(t1.mouse_points), 1)

        # Test sweep_all_key_methods and sweep_all_mouse_modes
        ok_k, fail_k, res_k = eng.sweep_all_key_methods(t1, test_key="F1")
        self.assertEqual(ok_k + fail_k, 22)
        self.assertEqual(len(res_k), 22)

        ok_m, fail_m, res_m = eng.sweep_all_mouse_modes(t1)
        self.assertEqual(ok_m + fail_m, 8)
        self.assertEqual(len(res_m), 8)

        # Add new tab & duplicate
        t2 = eng.add_new_tab(duplicate_from=0)
        self.assertIsNotNone(t2)
        self.assertEqual(len(eng.tabs), 2)
        self.assertEqual(len(t2.keys), 37)
        self.assertEqual(len(t2.mouse_points), 1)

        # Rename tab
        t2.title = "Boss Buffer Tab"
        self.assertEqual(t2.title, "Boss Buffer Tab")

        # Export & import workspace + profile
        with tempfile.TemporaryDirectory() as tmp:
            ws_path = str(Path(tmp) / "ws.json")
            self.assertTrue(eng.save_workspace(ws_path))
            self.assertTrue(eng.load_workspace(ws_path))
            self.assertEqual(len(eng.tabs), 2)

            prof_path = str(Path(tmp) / "tab.json")
            self.assertTrue(eng.save_tab_profile(eng.tabs[0], prof_path))
            loaded = eng.load_tab_profile(0, prof_path)
            self.assertTrue(loaded)
        if ws_def.exists():
            ws_def.unlink()

    def test_ps2_diagnostic_and_formatter(self) -> None:
        diag = diagnose_ps2_port_driver(run_live_probe=False)
        self.assertIn("state_code", diag)
        self.assertIn("inpout_dll_found", diag)
        self.assertIn("driver_open", diag)
        for is_ar in (True, False):
            h, d, k = format_ps2_diagnostic_report(diag, is_arabic=is_ar)
            self.assertTrue(len(h) > 5)
            self.assertTrue(len(d) > 10)
            self.assertIn(k, ("ok", "warn", "info"))

    def test_10_languages_have_all_autopresser_and_ps2_keys(self) -> None:
        required_keys = ['method_cpp_ps2_port', 'eng_status_ps2_active', 'eng_status_ps2_fallback', 'nav_autopresser', 'akp_title', 'akp_subtitle', 'akp_btn_stop_all_tabs', 'akp_btn_save_workspace', 'akp_btn_load_workspace', 'akp_ph_tab_name', 'akp_btn_new_tab', 'akp_btn_dup_tab', 'akp_btn_close_tab', 'akp_btn_choose_process', 'akp_lbl_run_in_bg', 'akp_btn_start_tab', 'akp_btn_stop_tab', 'akp_lbl_method', 'akp_btn_test_method', 'akp_btn_save_profile', 'akp_btn_load_profile', 'akp_sub_keys', 'akp_sub_mouse', 'akp_sub_log', 'akp_lbl_add_custom_key', 'akp_btn_arm_all', 'akp_btn_disarm_all', 'akp_test_single_key', 'akp_lbl_enable_mouse', 'akp_lbl_mouse_mode', 'akp_btn_add_mouse_pt', 'akp_lbl_activity_log', 'akp_log_started', 'akp_log_stopped', 'akp_log_no_keys', 'akp_log_sent_key', 'akp_method_sendinput', 'akp_method_scancode', 'akp_method_hybrid_hw', 'akp_method_ring0', 'akp_method_keybd_event', 'akp_method_unicode', 'akp_method_postmessage', 'akp_method_sendmessage', 'akp_method_post_child', 'akp_method_post_children', 'akp_method_post_active', 'akp_method_attach_state', 'akp_method_guithread_focus', 'akp_method_wm_char_triplet', 'akp_method_ime_pipeline', 'akp_method_sendnotify_tree', 'akp_mouse_mode_auto', 'akp_mouse_mode_real', 'akp_mouse_mode_post', 'akp_mouse_mode_post_top', 'akp_mouse_mode_hittest', 'akp_mouse_mode_attach', 'akp_mouse_mode_guithread', 'akp_mouse_mode_abs', 'lbl_ps2_diag_title', 'btn_check_ps2', 'btn_install_ps2', 'akp_btn_add_custom_key', 'akp_btn_capturing_custom_key', 'akp_toast_custom_key_added', 'akp_btn_test_all_methods', 'akp_btn_test_all_mouse_modes', 'akp_log_sweep_header', 'akp_log_sweep_item_ok', 'akp_log_sweep_item_fail', 'akp_log_sweep_summary', 'akp_lbl_tab_hotkey', 'akp_lbl_stop_all_hotkey']
        for code, _, _, _ in SUPPORTED_LANGUAGES:
            set_language(code)
            for rk in required_keys:
                val = t(rk)
                self.assertTrue(val and val != rk, f"Missing translation for {rk} in {code}")
        set_language("ar")



    def test_smart_combo_bridge_and_excluded_processes(self) -> None:
        from app.state import AppState
        from app.domain.models import SmartCombo, get_default_smart_combos
        st = AppState(AppConfig())
        st.smart_combos = get_default_smart_combos()
        eng = AutoKeyPresserEngine(st)
        matched = eng.find_matching_smart_combo("N")
        self.assertIsNotNone(matched)
        self.assertEqual(matched["trigger_key"], "N")

        sc = SmartCombo.from_dict({
            "id": "c_excl",
            "name": "Test Excl",
            "trigger_key": "N",
            "excluded_processes": ["notepad.exe", "chrome.exe"],
        })
        self.assertEqual(sc.excluded_processes, ["notepad.exe", "chrome.exe"])
        d = sc.to_dict()
        self.assertEqual(d["excluded_processes"], ["notepad.exe", "chrome.exe"])



    def test_python_mini_script_execution(self) -> None:
        from app.state import AppState
        from app.domain.macro_engine import MacroEngine
        st = AppState(AppConfig())
        eng = MacroEngine(st)
        # Execute a loop with print(), if/else, press(), and wait()
        script = (
            "total = 0\n"
            "for i in range(3):\n"
            "    if i >= 1:\n"
            "        print(f'Item {i}')\n"
            "    press('Enter')\n"
            "    wait(5)\n"
        )
        eng._execute_python_mini_script(script)




    def test_v390_per_method_python_sweep_and_background_modes(self) -> None:
        from app.domain.autopresser_engine import (
            AUTOPRESSER_KEY_METHODS,
            AUTOPRESSER_MOUSE_MODES,
            AutoKeyPresserEngine,
            AutoPresserMousePoint,
            AutoPresserTab,
            DEFAULT_PYTHON_SWEEP_SCRIPT,
            execute_per_method_python_test,
        )
        self.assertEqual(len(AUTOPRESSER_KEY_METHODS), 22)
        self.assertEqual(len(AUTOPRESSER_MOUSE_MODES), 8)
        mp_x1 = AutoPresserMousePoint.from_dict({"name": "Back", "x": 100, "y": 200, "button": "x1", "interval_ms": 500})
        mp_x2 = AutoPresserMousePoint.from_dict({"name": "Fwd", "x": 100, "y": 200, "button": "x2", "interval_ms": 500})
        self.assertEqual(mp_x1.button, "x1")
        self.assertEqual(mp_x2.button, "x2")

        ok, reason = execute_per_method_python_test(
            DEFAULT_PYTHON_SWEEP_SCRIPT,
            method_num=1,
            method_id="sendinput_scancode",
            method_name="Method 1",
            target_hwnd=0,
            run_in_background=True,
        )
        self.assertTrue(ok, reason)

        eng = AutoKeyPresserEngine()
        tab = AutoPresserTab(tab_id="test_bg_tab", title="BG Tab", target_process="notepad.exe", run_in_background=True)
        ok_k, fail_k, res_k = eng.sweep_all_key_methods(tab, test_key="F1")
        self.assertEqual(ok_k + fail_k, 22)
        self.assertEqual(len(res_k), 22)
        ok_m, fail_m, res_m = eng.sweep_all_mouse_modes(tab)
        self.assertEqual(ok_m + fail_m, 8)
        self.assertEqual(len(res_m), 8)

    def test_v390_smart_template_and_macro_sweep(self) -> None:
        from app.domain.autopresser_engine import (
            DEFAULT_TEMPLATE_SWEEP_STRING,
            execute_per_method_macro_test,
            expand_and_execute_method_template,
            format_diagnostic_reason,
        )
        ok, reason, preview = expand_and_execute_method_template(
            'key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \n',
            method_num=11,
            method_id="postmessage",
            method_name="Method 11: PostMessage",
            target_hwnd=0,
            target_app="notepad++.exe",
            run_in_background=True,
        )
        self.assertTrue(ok, reason)
        self.assertIn("[Enter]", preview)
        self.assertIn("11: Hello V-MacroPlus worked with postmessage", preview)

        demo_steps = [
            {"step_type": "key_tap", "value": "W", "delay_ms": 10, "hold_ms": 15},
            {"step_type": "type_text", "value": "Macro OK {method_id}", "delay_ms": 10},
        ]
        ok_m, reason_m, cnt = execute_per_method_macro_test(
            demo_steps,
            is_recording=False,
            method_num=16,
            method_id="attach_state",
            method_name="Method 16",
            target_hwnd=0,
            target_app="notepad++.exe",
            run_in_background=True,
            prefix_template=DEFAULT_TEMPLATE_SWEEP_STRING,
        )
        self.assertTrue(ok_m, reason_m)
        self.assertEqual(cnt, 3)

        diag_ar = format_diagnostic_reason(
            {
                "reason_code": "mouse_bg_ok",
                "method_id": "post",
                "btn": "left",
                "screen_x": 486,
                "screen_y": 486,
                "client_x": 312,
                "client_y": 190,
                "child_cls": "Scintilla",
            }
        )
        self.assertIn("486", diag_ar)
        self.assertIn("Scintilla", diag_ar)

    def test_v395_humanized_helpers_and_saved_python_presets(self) -> None:
        from app.domain.autopresser_engine import execute_per_method_python_test
        from app.domain.python_presets import (
            BUILTIN_PYTHON_PRESETS,
            delete_user_python_script,
            get_python_script_code_by_key,
            get_python_script_dropdown_items,
            upsert_user_python_script,
        )
        self.assertEqual(len(BUILTIN_PYTHON_PRESETS), 6)
        saved_id, saved_nm = upsert_user_python_script("My Custom 3D Combo", 'human_press("1", 10, 15, 5, 10)')
        self.assertTrue(saved_id.startswith("user:"))
        self.assertEqual(saved_nm, "My Custom 3D Combo")
        code = get_python_script_code_by_key(saved_id)
        self.assertIn("human_press", code or "")
        items = get_python_script_dropdown_items()
        self.assertTrue(any(k == saved_id for k, _ in items))
        self.assertTrue(delete_user_python_script(saved_id))

        script = "\n".join([
            'press("w", hold_ms=rand(15, 25), human=True)',
            'human_wait(5, 10)',
            'human_press("Tab", min_hold=15, max_hold=20, min_wait=5, max_wait=10)',
            'hold_key("Space", duration_ms=40)',
            'move_rel(15, 0, steps=2, duration_ms=10)',
            'click("left", hold_ms=20, human=True, in_game_3d=True)',
            'print("OK", enter=True, human=True)',
        ])
        ok, reason = execute_per_method_python_test(
            script,
            method_num=1,
            method_id="sendinput_scancode",
            method_name="Method 1",
            target_hwnd=0,
            run_in_background=False,
        )
        self.assertTrue(ok, reason)



    def test_step_enabled_and_note_preservation_and_stop_hotkey(self) -> None:
        from app.domain.models import ComboStep, MacroEvent, SmartCombo
        from app.domain.autopresser_engine import execute_per_method_macro_test

        ev = MacroEvent(
            kind="key_tap",
            delay_ms=25,
            hold_ms=40,
            key="W",
            scan_code=0x11,
            enabled=False,
            note="Paused W movement step for testing",
        )
        d_ev = ev.to_dict()
        ev_reloaded = MacroEvent.from_dict(d_ev)
        self.assertFalse(ev_reloaded.enabled)
        self.assertEqual(ev_reloaded.note, "Paused W movement step for testing")

        st = ComboStep.from_macro_event(ev_reloaded)
        self.assertFalse(st.enabled)
        self.assertEqual(st.note, "Paused W movement step for testing")

        sc = SmartCombo(
            id="combo_rep_test",
            name="Repeat Test",
            trigger_key="F2",
            mode="toggle_loop",
            stop_hotkey="F4",
            enabled=True,
            steps=[
                st,
                ComboStep(step_type="key_tap", value="Space", delay_ms=10, enabled=True, note="Active jump step"),
            ],
        )
        sc_reloaded = SmartCombo.from_dict(sc.to_dict())
        self.assertEqual(sc_reloaded.stop_hotkey, "F4")
        self.assertEqual(len(sc_reloaded.steps), 2)
        self.assertFalse(sc_reloaded.steps[0].enabled)
        self.assertTrue(sc_reloaded.steps[1].enabled)
        self.assertEqual(sc_reloaded.steps[1].note, "Active jump step")

        ok, reason, count = execute_per_method_macro_test(
            [s.to_dict() for s in sc_reloaded.steps],
            is_recording=False,
            method_num=1,
            method_id="sendinput_scancode",
            method_name="Method 1",
            target_hwnd=0,
            target_app="Sandbox",
            run_in_background=False,
            prefix_template="",
            allow_self_test=True,
        )
        self.assertTrue(ok)
        self.assertEqual(count, 1)

    def test_self_protection_functions_exist(self) -> None:
        from app.domain.win32_input import OUR_PID, is_hwnd_own_process, is_own_app_foreground
        self.assertTrue(OUR_PID > 0)
        self.assertFalse(is_hwnd_own_process(0))
        self.assertIsInstance(is_own_app_foreground(), bool)



    def test_v396_mcu_bridge_pico_and_arduino_firmwares_and_diagnostics(self) -> None:
        from app.domain.microcontroller_bridge import (
            diagnose_mcu_boards,
            ensure_firmware_files_on_disk,
            format_mcu_diagnostic_report,
            list_serial_ports_with_mcu_info,
        )
        from app.domain.win32_input import resolve_effective_input_engine

        fw_root = ensure_firmware_files_on_disk()
        self.assertTrue((fw_root / "raspberry_pi_pico_circuitpython" / "boot.py").exists())
        self.assertTrue((fw_root / "raspberry_pi_pico_circuitpython" / "code.py").exists())
        self.assertTrue((fw_root / "raspberry_pi_pico_arduino_cpp" / "pico_hid_bridge.ino").exists())
        self.assertTrue((fw_root / "arduino_leonardo_promicro" / "arduino_hid_bridge.ino").exists())

        ports = list_serial_ports_with_mcu_info()
        self.assertIsInstance(ports, list)

        diag = diagnose_mcu_boards(preferred_port="auto", run_live_ping=False)
        h_ar, d_ar, k_ar = format_mcu_diagnostic_report(diag, is_arabic=True)
        h_en, d_en, k_en = format_mcu_diagnostic_report(diag, is_arabic=False)
        self.assertTrue(len(h_ar) > 10 and len(d_ar) > 10)
        self.assertTrue(len(h_en) > 10 and len(d_en) > 10)
        self.assertIn(k_ar, ("ok", "warn", "info"))

        eff_pico = resolve_effective_input_engine("pico_usb_hid", is_arabic=True)
        self.assertEqual(eff_pico["requested_method"], "pico_usb_hid")
        eff_ard = resolve_effective_input_engine("arduino_usb_hid", is_arabic=False)
        self.assertEqual(eff_ard["requested_method"], "arduino_usb_hid")


if __name__ == "__main__":
    unittest.main()
