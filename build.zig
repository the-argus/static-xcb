const std = @import("std");
const xcb_build_zig = @import("xcb");
const zcc = @import("compile_commands");

pub fn build(b: *std.Build) void {
    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);
    defer targets.deinit();
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const xproto_header_dir = b.option([]const u8, "xproto_header_dir", "header directory to use for libX11") orelse "";

    const xcb = b.dependency("xcb", .{
        .target = target,
        .optimize = optimize,
        .xproto_header_dir = xproto_header_dir,
    }).artifact("xcb");

    const exe = b.addExecutable(.{
        .name = "example",
        .target = target,
        .optimize = optimize,
    });
    targets.append(exe) catch @panic("OOM");

    exe.addCSourceFiles(&.{
        "src/main.c",
    }, &.{
        "-Wall",
    });

    exe.addIncludePath(.{ .path = "include" });

    exe.linkLibC();
    exe.linkLibrary(xcb);

    const chipmunk = b.dependency("chipmunk2d", .{
        .target = target,
        .optimize = optimize,
    });
    exe.linkLibrary(chipmunk.artifact("chipmunk"));

    b.installArtifact(exe);

    zcc.createStep(b, "cdb", targets.toOwnedSlice() catch @panic("OOM"));
}

fn regenerateHeadersMakeFn(step: *std.Build.Step, prog_node: *std.Progress.Node) anyerror!void {
    _ = prog_node;
    xcb_build_zig.regenerateHeaders(step.owner);
}
